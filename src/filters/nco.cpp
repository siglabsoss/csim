#include <filters/nco.hpp>

NCO::NCO(double freq) :
    _cos(NCO::TBSIZE),
    _sin(NCO::TBSIZE),
    _phase_acc(0.0),
    _dither_lfsr_memory(0x5678), //something nonzero
    _phase_increment(-freq)
{
    // Initialize Sin/Cos LUTs
    for (int i = 0; i < NCO::TBSIZE; i++) {
        _cos[i] = std::cos(2.0 * M_PI * i / double(NCO::TBSIZE));
        _sin[i] = std::sin(2.0 * M_PI * i / double(NCO::TBSIZE));
    }
}

// Push two more samples in, and report if there is an output available
void NCO::pullNextSample(
        // Outputs
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & cosine_out,
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & sine_out)
{
    // Constants
    SLFixedPoint<TWOPIWIDTH, 3, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> FPM_2PI = 2.0 * M_PI;
    // Fixed point local variables
    SLFixedPoint<PWIDTH,  0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND>                                dithered_phase_acc;
    SLFixedPoint<PWIDTH,  0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND>                                phase_corr;
    SLFixedPoint<PWIDTH,  0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                                dither_amount;
    SLFixedPoint<PWIDTH,  0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                                dithered_phase_corr;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                                lut_sine;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>                                lut_cosine;

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
    lut_sine   = _sin[phase_index_int];

    //dithered_phase_corr = phase_corr + dither_amount;
    dithered_phase_corr = FPM_2PI * phase_corr;

    // Apply correction
    sine_out = lut_sine - lut_cosine * dithered_phase_corr;
    cosine_out = lut_cosine + lut_sine * dithered_phase_corr;

    // Update Phase Accumulator
    _phase_acc += _phase_increment;
}
