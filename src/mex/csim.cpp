#include <stdio.h>
#include <mex.h>
#include <stdint.h>

#include <vector>
#include <utility>

extern "C"
{
#include "csim.hpp"
};

#include <filters/modulator.hpp>
#include <filters/mixer.hpp>
#include <filters/decimator.hpp>
#include <filters/hard_demod.hpp>
#include <core/radio_s.hpp>
#include <core/radio_set.hpp>

static constexpr size_t UPSAMPLE_FACTOR = 1;
static constexpr size_t MOD_TICKS_PER_SYMBOL = 4;
//ticks per symbol needs to be greater than upsample factor in order to not saturate
static constexpr size_t MIXER_TICKS_PER_PERIOD = (MOD_TICKS_PER_SYMBOL / UPSAMPLE_FACTOR) * 1;

static void constructModulationChain(FilterChain &modChain, Modulator::mod_scheme_t scheme)
{
    Modulator           *qam16    = new Modulator(MOD_TICKS_PER_SYMBOL, scheme);
    Mixer *upmixer                = new Mixer(MIXER_TICKS_PER_PERIOD, true /* upmix */);
    modChain = *upmixer + *qam16;
}

static void constructDemodulationChain(FilterChain &demodChain, Modulator::mod_scheme_t scheme)
{
    Mixer *downmixer = new Mixer(MIXER_TICKS_PER_PERIOD, false /* downmix */);
    Decimator *decim = new Decimator(MOD_TICKS_PER_SYMBOL, 0);
    HardDemod *demod = new HardDemod(scheme, 0.0);
    demodChain = *demod + *decim + *downmixer;
}

static struct csim_context_t
{
    RadioSet *                          rs = new RadioSet;
    size_t                              radio_count = 0;
    std::vector<std::queue<uint8_t> >   rx_bytes;
} g_context;

static void construct_radios(const std::vector <std::pair<double, double> > &coords)
{
    Modulator::mod_scheme_t scheme = Modulator::MOD_SCHEME_QAM16;
    for (auto it = coords.begin(); it != coords.end(); it++) {
        size_t count = g_context.radio_count;
        g_context.rs->addRadio([scheme, it, count]()
                {
                    radio_config_t config {
                        .position = Vector2d(it->first, it->second),
                        .id = static_cast<radio_id_t>(count) //NOTE the code in this file is dependent on sequential IDs starting from 0
                    };

                    FilterChain modulation_chain;
                    constructModulationChain(modulation_chain, scheme);

                    FilterChain demodulation_chain;
                    constructDemodulationChain(demodulation_chain, scheme);

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                });
        g_context.radio_count++;
    }
    g_context.rs->init(true, true, true);
    g_context.rx_bytes = std::vector<std::queue<uint8_t> >(g_context.radio_count);

    mexPrintf("Constructed %d radio\n", g_context.radio_count);
}

/**************************** C-ABI hooks for MATLAB ****************************/

void csim_init_radios(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    mexPrintf("nlhs = %d\tnrhs = %d\n", nlhs, nrhs);
    if (nrhs != 1) {
        mexPrintf("One argument required: array of 2D positions in meters (X, Y)\n");
        return;
    }
    if (mxIsScalar(prhs[0])) {
        mexPrintf("Argument cannot be scalar\n");
        return;
    }

    if (mxGetNumberOfDimensions(prhs[0]) != 2) {
        mexPrintf("Argument must be exactly 2-dimensional\n");
        return;
    }

    const mwSize *dimSize = mxGetDimensions(prhs[0]);
    const mwSize rowSize = dimSize[0];
    const mwSize colSize = dimSize[1];
    if (colSize != 2) {
        mexPrintf("Argument must have exactly two columns.\n");
        return;
    }

    if (!mxIsDouble(prhs[0])) {
        mexPrintf("Array must contain double-precision floating point values.\n");
        return;
    }
    double *vals = mxGetPr(prhs[0]);
    std::vector< std::pair<double, double> > coords;
    if (vals != nullptr) {
        for (mwSize row = 0; row < rowSize; row++) {
            double x = vals[rowSize*0 + row];
            double y = vals[rowSize*1 + row];
            mexPrintf("(%f,%f)\n", x, y);
            coords.push_back(std::pair<double, double>(x, y));
        }
        construct_radios(coords);
    } else {
        mexPrintf("Could not obtain doubles from array!\n");
    }
}

void csim_get_rx_bytes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
}


void csim_set_tx_byte(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    bool invalArgs = false;
    if (nrhs != 2 || !mxIsScalar(prhs[0]) || !mxIsScalar(prhs[1]) || !mxIsDouble(prhs[0]) || !mxIsDouble(prhs[1])) {
        invalArgs = true;
    }

    radio_id_t id = static_cast<radio_id_t>(mxGetScalar(prhs[0]));
    unsigned byte = static_cast<unsigned>(mxGetScalar(prhs[1]));

    if (id >= g_context.radio_count || byte > 255) {
        invalArgs = true;
    }

    if (invalArgs == true) {
        mexPrintf("Expecting exactly two arguments: radio_id ([0-numRadios-1]), byte ([0-255])");
        return;
    }

    RadioS *radio = g_context.rs->getRadioForId(id);
    assert(radio != nullptr);
    radio->txByte(byte);
}

void csim_tick(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    bool invalArgs = false;

    if (nlhs > 1) {
        mexPrintf("Too many output arguments...\n");
        return;
    }

    if ( (nrhs != 1) || (mxGetNumberOfDimensions(prhs[0]) != 2)) {
        mexPrintf("Invalid argument count or dimensions\n");
        invalArgs = true;
    }

    if (invalArgs == false) {
        const mwSize *dimSize = mxGetDimensions(prhs[0]);
        const mwSize rowSize = dimSize[0];
        const mwSize colSize = dimSize[1]; //this is OK because dimSize is guaranteed to be >= 2 in length
        if (colSize != 1 || rowSize != g_context.radio_count) {
            mexPrintf("Invalid size\n");
            invalArgs = true;
        }
    }

    if (invalArgs == false) {
        if (!mxIsDouble(prhs[0])) {
            mexPrintf("Invalid class of values");
            invalArgs = true;
        }
    }

    if (invalArgs == true) {
        mexPrintf("Must supply exactly one 2D Matrix of complex values with dimensions 1 x N (N = number of radios)\n");
        return;
    }
    double *reals = mxGetPr(prhs[0]);
    double *imags = mxGetPi(prhs[0]);

    plhs[0] = mxCreateDoubleMatrix(g_context.radio_count, 1, mxCOMPLEX);

    size_t i = 0;
    for (RadioSet::iterator it = g_context.rs->begin(); it != g_context.rs->end(); it++)
    {
        RadioS * const current = (*it).get();
        double real = reals[i];
        double imag = imags != nullptr ? imags[i] : 0.0;
        ComplexDouble rxSample(real, imag);
        ComplexDouble txSample;

        /* Step 1 - Feed the radio a waveform sample passed in from MATLAB */
        mexPrintf("rxSample (%f,%f)\n", rxSample.real(), rxSample.imag());
        current->rxWave(rxSample);

        /* Step 2 - trigger the radio's internal processing */
        current->tick();

        /* Step 3 - Get the current radio's RF output to pass back to MATLAB */
        current->txWave(txSample);

        double *outr = mxGetPr(plhs[0]);
        double *outi = mxGetPr(plhs[0]);
        outr[i] = txSample.real();
        outi[i] = txSample.imag();

        /* Step 4 - Check for output bytes */
        uint8_t outputByte;
        if (current->rxByte(outputByte)) {
            size_t offset = static_cast<size_t>(current->getId());
            g_context.rx_bytes[offset].push(outputByte);
            mexPrintf("Radio %d received byte %x\n", offset, outputByte);
        }
        i++;
    }
}

