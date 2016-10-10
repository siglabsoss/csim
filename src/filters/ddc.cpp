#include <iostream>
#include <fstream>
#include <complex>
#include <cmath>

#include "/home/behrooze/Work/csim/src/types/fixedpoint.hpp"

// Number of iterations to run simulation
constexpr int MAX_ITERS  = 200000;

// In implementation it may be helpful to exploint half-wave and 
// quarter-wave symmetry to reduce storage requirements. In that
// case a table of 1024 samples requires only 256 samples of storage.
constexpr int TBSIZE     = 1024; // TBSIZE of 4*8192 guarantees ~120 dB dynamic range
constexpr int LOG2TBSIZE = std::ceil(std::log(TBSIZE) / std::log(2));
constexpr int TBWIDTH    = 18;
constexpr int PWIDTH     = 32;
constexpr int INWIDTH    = 16;
constexpr int OUTWIDTH   = 18; // Easiest way to remove small DC bias is to increaase this (22 works)

// Filter coefficient files and lengths
constexpr char HB_FILENAME[] = "halfband.txt";
constexpr int  HB_LENGTH     = 71;
constexpr char B5_FILENAME[] = "downby5.txt";
constexpr int  B5_LENGTH     = 378;

class DigitalDownConverter
{
    // Digital Synthesis of Sinusoid
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> _cos[TBSIZE];
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> _sin[TBSIZE];
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND> _phase_acc;
    unsigned long                               _dither_lfsr_memory;

    // Decimation Filter (by 2)
    SLFixedPoint<18, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>       _halfband_coeffs[HB_LENGTH];
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> _halfband_inph_delays[HB_LENGTH];
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> _halfband_quad_delays[HB_LENGTH];
    unsigned long             _halfband_iteration;

    // Decimation Filter (by 5)
    SLFixedPoint<18, -1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>     _by5_coeffs[B5_LENGTH];
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> _by5_inph_delays[B5_LENGTH];
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> _by5_quad_delays[B5_LENGTH];
    unsigned long             _by5_iteration;

public:
    DigitalDownConverter();
    bool push(
        // Inputs
        SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> input_sample,
        SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  phase_increment,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out);
    bool pull_sinusoid(
        // Inputs
        SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>    phase_increment,
        // Outputs
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & cosine_out,
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & sine_out);
    bool push_halfband(
        // Inputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  inph_in,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out);
    bool push_by5(
        // Inputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>   inph_in,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>   quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out);
};

int main(int argc, char * argv[])
{
    SLFixPoint::throwOnOverflow = true;
    double frequency = 0.16;
    SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> input_sample;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> phase_increment;
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> inph;
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> quad;

    auto ddc = DigitalDownConverter();
    phase_increment = -frequency;

    for (int ii = 0; ii < MAX_ITERS; ++ii) {
        input_sample = 0.1
            + 0.125 * std::cos(2*M_PI*(frequency + 0.005)*ii + 0.72)
            + 0.125 * std::cos(2*M_PI*(0.5 * frequency)*ii)
            + 0.125 * std::cos(2*M_PI*(frequency - 0.005)*ii + 0.72)
            + 0.125 * std::cos(2*M_PI*(frequency + 0.01)*ii - 1.333)
            + 0.125 * std::cos(2*M_PI*(0.25 * frequency)*ii)
            + 0.125 * std::cos(2*M_PI*(frequency - 0.01)*ii - 1.333);

        if (ddc.push(input_sample, phase_increment, inph, quad)) {
            std::cout << inph.to_double() << ", " << quad.to_double() << ", " << std::endl;
        }
    }
    
    return 0;
}

DigitalDownConverter::DigitalDownConverter(void) : 
    _phase_acc(0.0)
{
    // Initialize Sin/Cos LUTs
    for (int i = 0; i < TBSIZE; i++) {
        _cos[i] = std::cos(2.0 * M_PI * i / double(TBSIZE));
        _sin[i] = std::sin(2.0 * M_PI * i / double(TBSIZE));
    }

    int c;
    std::ifstream halfband_file(HB_FILENAME);
    double raw_coeffs[std::max(HB_LENGTH,B5_LENGTH)];
    double coeff_accum = 0.0;
    for (int i = 0; i < HB_LENGTH; i++) {
        halfband_file >> c;
        coeff_accum += c;
        raw_coeffs[i] = double(c);
        _halfband_inph_delays[i] = 0.0;
        _halfband_quad_delays[i] = 0.0;
    }
    double max = 0.0;
    double min = 1.0;
    for (int i = 0; i < HB_LENGTH; i++) {
        double coeff = raw_coeffs[i] * (1.0 / coeff_accum);
        _halfband_coeffs[i] = coeff;
        if (fabs(coeff) > max) {
            max = fabs(coeff);
        }
        if (fabs(coeff) < min && fabs(coeff) != 0.0) {
            min = fabs(coeff);
        }
    }
//    std::cout << "max = " << max << " min = " << min << std::endl;
    _halfband_iteration = 0;
    halfband_file.close();

    std::ifstream by5_file(B5_FILENAME);
    coeff_accum = 0.0;
    for (int i = 0; i < B5_LENGTH; i++) {
        by5_file >> c;
        coeff_accum += c;
        raw_coeffs[i] = double(c);
        _by5_inph_delays[i] = 0.0;
        _by5_quad_delays[i] = 0.0;
    }
    max = 0.0;
    min = 1.0;
    for (int i = 0; i < B5_LENGTH; i++) {
        double coeff = raw_coeffs[i] * (1.0 / coeff_accum);
        _by5_coeffs[i] = coeff;
        if (fabs(coeff) > max) {
            max = fabs(coeff);
        }
        if (fabs(coeff) < min && fabs(coeff) != 0.0) {
            min = fabs(coeff);
        }
    }
//    std::cout << "max = " << max << " min = " << min << std::endl;
    _by5_iteration = 0;
    by5_file.close();

    // Initialize LFSR to something nonzero
    _dither_lfsr_memory = 0x5678;
}

bool DigitalDownConverter::push(
        // Inputs
        SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>    input_sample,
        SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>     phase_increment,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> cosine;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> sine;

    if (pull_sinusoid(phase_increment, cosine, sine)) {
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> bb_inph;
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> bb_quad;
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> half_inph;
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> half_quad;

        bb_inph = input_sample * cosine;
        bb_quad = input_sample * sine;

        // std::cout << double(bb_inph) << ", " << double(bb_quad) << ", " << std::endl;

        if (push_halfband(bb_inph, bb_quad, half_inph, half_quad)) {
            SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> by5_inph;
            SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> by5_quad;

            // std::cout << double(half_inph) << ", " << double(half_quad) << ", " << std::endl;

            if (push_by5(half_inph, half_quad, by5_inph, by5_quad)) {
                inph_out = by5_inph;
                quad_out = by5_quad;
                output_ready = true;

                // std::cout << double(by5_inph) << ", " << double(by5_quad) << ", " << std::endl;
            }
        }
    }

    return output_ready;
}

// Push two more samples in, and report if there is an output available
bool DigitalDownConverter::pull_sinusoid(
        // Inputs
        SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>    phase_increment,
        // Outputs
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & cosine_out,
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & sine_out)
{
    // Constants
    SLFixedPoint<OUTWIDTH, 3, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> FPM_2PI = 2.0 * M_PI;
    // Fixed point local variables
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND>               dithered_phase_acc;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND>               phase_corr;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                                dither_amount;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                                dithered_phase_corr;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                               lut_sine;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                               lut_cosine;
    // Indicates whether an output is available to be read.
    bool output_ready = false;

    // Implement an LFSR for the dithering operation
    unsigned long lfsr_bit = ((_dither_lfsr_memory >> 31) ^ (_dither_lfsr_memory >> 28)) & 0x1;
    _dither_lfsr_memory = ((_dither_lfsr_memory << 1) & 0xFFFFFFFF) | lfsr_bit;
    if ((_dither_lfsr_memory & 0x1) == 0) {
        // Grab 20 bits, and place them at half significance of the phase_correction
        dither_amount = -double(_dither_lfsr_memory & 0xFFFFF) * std::pow(2, -LOG2TBSIZE - 21);
    }
    else {
        dither_amount = double(_dither_lfsr_memory & 0xFFFFF) * std::pow(2, -LOG2TBSIZE - 21);
    }
    dithered_phase_acc = _phase_acc + dither_amount;

    // Slice the phase accumulator into a phase index (for lookup) and a phase correction
    //phase_index = dithered_phase_acc.slice(PWIDTH-1, PWIDTH-LOG2TBSIZE);
    phase_corr = double(dithered_phase_acc.slice(PWIDTH-LOG2TBSIZE-1, 0)) * std::pow(2.0, -PWIDTH);

    // Lookup values of sine and cosine in the tables
    uint64_t phase_index_int = dithered_phase_acc.slice(PWIDTH-1, PWIDTH-LOG2TBSIZE);
    lut_cosine = _cos[phase_index_int];
    lut_sine = _sin[phase_index_int];
    
    //dithered_phase_corr = phase_corr + dither_amount;
    dithered_phase_corr = FPM_2PI * phase_corr;

    // Apply correction
    sine_out = lut_sine - lut_cosine * dithered_phase_corr;
    cosine_out = lut_cosine + lut_sine * dithered_phase_corr;
    output_ready = true;

    // Update Phase Accumulator
    _phase_acc += phase_increment;

    return output_ready;    
}

bool DigitalDownConverter::push_halfband(
        // Inputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  inph_in,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<52, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  inph_accum = 0.0;
    SLFixedPoint<52, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  quad_accum = 0.0;

    // Rotate delay line
    for (int i = HB_LENGTH-1; i > 0; i--) {
        _halfband_inph_delays[i] = _halfband_inph_delays[i - 1];
        _halfband_quad_delays[i] = _halfband_quad_delays[i - 1];
    }
    _halfband_inph_delays[0] = inph_in;
    _halfband_quad_delays[0] = quad_in;

    // Swap iteration number
    if (_halfband_iteration == 0) {
        _halfband_iteration = 1;
    }
    // Compute output and swap iteration number
    else {
        _halfband_iteration = 0;

        inph_accum = _halfband_inph_delays[(HB_LENGTH-1)/2] * _halfband_coeffs[(HB_LENGTH-1)/2];
        quad_accum = _halfband_quad_delays[(HB_LENGTH-1)/2] * _halfband_coeffs[(HB_LENGTH-1)/2];
        for (int i = 0; i < HB_LENGTH; i += 2) {
            inph_accum += _halfband_inph_delays[i] * _halfband_coeffs[i];
            quad_accum += _halfband_quad_delays[i] * _halfband_coeffs[i];
        }
        //inph_out = inph_accum * _halfband_normalization;
        //quad_out = quad_accum * _halfband_normalization;
        inph_out = inph_accum;
        quad_out = quad_accum;

        // Indicate that the output is ready
        output_ready = true;
    }

    return output_ready;
}

bool DigitalDownConverter::push_by5(
        // Inputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  inph_in,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<52, -1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  inph_accum = 0.0;
    SLFixedPoint<52, -1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>  quad_accum = 0.0;

    // Rotate delay line
    for (int i = B5_LENGTH-1; i > 0; i--) {
        _by5_inph_delays[i] = _by5_inph_delays[i-1];
        _by5_quad_delays[i] = _by5_quad_delays[i-1];
    }
    _by5_inph_delays[0] = inph_in;
    _by5_quad_delays[0] = quad_in;

    // Polyphase Decimation Filter Computation
    switch(_by5_iteration) {
        case 4:
            // Compute output (use polyphase structure in hardware implementation)
            inph_accum = 0.0;
            quad_accum = 0.0;
            for (int i = 0; i < B5_LENGTH; i++) {
                inph_accum += _by5_inph_delays[i] * _by5_coeffs[i];
                quad_accum += _by5_quad_delays[i] * _by5_coeffs[i];
            }
            //inph_out = inph_accum * _by5_normalization;
            //quad_out = quad_accum * _by5_normalization;
            inph_out = inph_accum;
            quad_out = quad_accum;
            output_ready = true;
            
            // Update iteration index
            _by5_iteration = 0;

            break;
        default:
            // Update iteration index
            _by5_iteration++;
            break;
    }

    return output_ready;
}
