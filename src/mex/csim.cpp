#include <stdio.h>
#include <mex.h>
#include <stdint.h>

#include <vector>
#include <utility>
#include <queue>

extern "C"
{
#include "csim.h"
#include "mex_utils.h"
};

#include <core/sigworld.hpp>
#include <core/radio_s.hpp>
#include <core/radio_set.hpp>
#include <utils/radio_utils.hpp>

static struct csim_context_t
{
    SigWorld *                          world = new SigWorld;
    RadioSet *                          rs = new RadioSet;
    size_t                              radio_count = 0;
    std::vector<std::queue<uint8_t> >   rx_bytes;
} g_context;

static void construct_radios_from_coords(const std::vector <std::pair<double, double> > &coords)
{
    g_context.world->reset();
    Modulator::mod_scheme_t scheme = Modulator::MOD_SCHEME_QAM16;
    construct_radio_set(*g_context.rs, coords, scheme);
    g_context.radio_count = coords.size();
    g_context.rx_bytes = std::vector<std::queue<uint8_t> >(g_context.radio_count);
    g_context.world->init(g_context.rs);

    //Define callback for receiving bytes
    g_context.world->didReceiveByte([](radio_id_t id, uint8_t byte){
        size_t offset = static_cast<size_t>(id);
        g_context.rx_bytes[offset].push(byte);
        mexPrintf("Radio %d received byte %d\n", offset, byte);
    });
}

/**************************** C-ABI hooks for MATLAB ****************************/

void csim_init_radios(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (!check_for_double_2d_matrix_with_colsize(2, nrhs, prhs)) {
        mexErrMsgIdAndTxt("csim:init:InvalidArgs", "Must provide an Nx2 matrix of 2D coordinates");
        return;
    }
    const mwSize *dimSize = mxGetDimensions(prhs[0]);
    const mwSize rowSize = dimSize[0];

    double *vals = mxGetPr(prhs[0]);
    std::vector< std::pair<double, double> > coords;
    if (vals != nullptr) {
        for (mwSize row = 0; row < rowSize; row++) {
            double x = vals[rowSize*0 + row];
            double y = vals[rowSize*1 + row];
            //mexPrintf("(%f,%f)\n", x, y);
            coords.push_back(std::pair<double, double>(x, y));
        }
        construct_radios_from_coords(coords);
    }
}

void csim_get_rx_bytes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    bool invalArgs = false;
    if(!check_for_scalars(1, nrhs, prhs)) {
        invalArgs = true;
    }

    if (!invalArgs) {
        radio_id_t id = static_cast<radio_id_t>(mxGetScalar(prhs[0]));

        if (id >= g_context.radio_count) {
            invalArgs = true;
        } else {
            size_t offset = static_cast<size_t>(id);
            if (!g_context.rx_bytes[offset].empty()) {
                uint8_t byte = g_context.rx_bytes[offset].front();
                g_context.rx_bytes[offset].pop();
                plhs[0] = mxCreateDoubleScalar(byte);
            }
        }
    }

    if (invalArgs == true) {
        mexErrMsgIdAndTxt("csim:set_tx_byte:InvalidArgs", "Expecting exactly two arguments: radio_id ([0-numRadios-1]), byte ([0-255])");
        return;
    }
}

void csim_set_tx_byte(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    bool invalArgs = false;
    if(!check_for_scalars(2, nrhs, prhs)) {
        invalArgs = true;
    }

    if (!invalArgs) {
        radio_id_t id = static_cast<radio_id_t>(mxGetScalar(prhs[0]));
        unsigned byte = static_cast<unsigned>(mxGetScalar(prhs[1]));

        if (id >= g_context.radio_count || byte > 255) {
            invalArgs = true;
        } else {
            g_context.world->sendByte(id, static_cast<uint8_t>(byte));
        }
    }

    if (invalArgs == true) {
        mexErrMsgIdAndTxt("csim:set_tx_byte:InvalidArgs", "Expecting exactly two arguments: radio_id ([0-numRadios-1]), byte ([0-255])");
        return;
    }
}

void csim_tick(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs > 1) {
        mexPrintf("Too many output arguments...\n");
        return;
    }
    if (!check_for_double_col_vector_of_length(g_context.radio_count, nrhs, prhs)) {
        mexErrMsgIdAndTxt("csim:tick:InvalidArgs", "Must supply exactly one 2D Matrix of complex values with dimensions 1 x N (N = number of radios)\n");
        return;
    }

    double *reals = mxGetPr(prhs[0]);
    double *imags = mxGetPi(prhs[0]);

    plhs[0] = mxCreateDoubleMatrix(g_context.radio_count, 1, mxCOMPLEX);
    double *outr = mxGetPr(plhs[0]);
    double *outi = mxGetPi(plhs[0]);

    //Define lambda to retrieve the inputs
    g_context.world->setExternalDataSource([&reals, &imags](const RadioS *current, ComplexDouble &data) {
        size_t i = static_cast<size_t>(current->getId()); //this assumes radio ids were sequential, starting from 0
        //mexPrintf("Sigworld requesting data from external source for radio %d\n", i);
        data.real(reals[i]);
        data.imag(imags != nullptr ? imags[i] : 0.0);
        if (reals[i] > 0.0 || (imags != nullptr && imags[i] > 0.0)) {
            //mexPrintf("Radio %d received nonzero data! (%f, %f)\n", i, reals[i], imags[i]);
        }
    });

    //Define lambda to retrieve the outputs
    g_context.world->setExternalDataDestination([&outr, &outi](const RadioS *current, const ComplexDouble &data) {
        size_t i = static_cast<size_t>(current->getId()); //this assumes radio ids were sequential, starting from 0
        //mexPrintf("Sigworld pushing data to external source for radio %d\n", i);
        outr[i] = data.real();
        outi[i] = data.imag();
//        if (outr[i] > 0.0 || outi[i] > 0.0) {
//            mexPrintf("Radio %d pushing nonzero data! (%f, %f)\n", i, outr[i], outi[i]);
//        }
    });

    g_context.world->tick();
}

