#include <filters/ddc.hpp>


DigitalDownConverter::DigitalDownConverter(double freq, const std::vector<double> &halfbandCoeffs, const std::vector<double> &by5Coeffs) :
    FilterChainElement("DDC"),
    _cos(TBSIZE),
    _sin(TBSIZE),
    _phase_acc(0.0),
    _dither_lfsr_memory(0x5678), //something nonzero
    _phase_increment(-freq),
    _halfband_coeffs(),
    _halfband_inph_delays(),
    _halfband_quad_delays(),
    _halfband_iteration(0),
    _by5_coeffs(),
    _by5_inph_delays(),
    _by5_quad_delays(),
    _by5_iteration(0),
    _output_ready(false),
    _output_inph(0.0),
    _output_quad(0.0)
{
    // Initialize Sin/Cos LUTs
    for (int i = 0; i < TBSIZE; i++) {
        _cos[i] = std::cos(2.0 * M_PI * i / double(TBSIZE));
        _sin[i] = std::sin(2.0 * M_PI * i / double(TBSIZE));
    }

    double coeff_accum = 0.0;
    for (size_t i = 0; i < halfbandCoeffs.size(); i++) {
        coeff_accum += halfbandCoeffs[i];
    }
    double coeff_scale = (1.0 / coeff_accum);
    _halfband_coeffs.resize(halfbandCoeffs.size());
    _halfband_inph_delays.resize(halfbandCoeffs.size());
    _halfband_quad_delays.resize(halfbandCoeffs.size());
    for (size_t i = 0; i < halfbandCoeffs.size(); i++) {
        _halfband_coeffs[i] = halfbandCoeffs[i] * coeff_scale;
        _halfband_inph_delays[i] = 0.0;
        _halfband_quad_delays[i] = 0.0;
    }

    coeff_accum = 0.0;
    for (size_t i = 0; i < by5Coeffs.size(); i++) {
        coeff_accum += by5Coeffs[i];
    }
    coeff_scale = (1.0 / coeff_accum);
    _by5_coeffs.resize(by5Coeffs.size());
    _by5_inph_delays.resize(by5Coeffs.size());
    _by5_quad_delays.resize(by5Coeffs.size());
    for (size_t i = 0; i < by5Coeffs.size(); i++) {
        _by5_coeffs[i] = by5Coeffs[i] * coeff_scale;
        _by5_inph_delays[i] = 0.0;
        _by5_quad_delays[i] = 0.0;
    }
}

bool DigitalDownConverter::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> input = data.fc.real();
    _output_ready = push(input, _output_inph, _output_quad);
    return true;
}

bool DigitalDownConverter::output(filter_io_t &data)
{
    if (_output_ready) {
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(_output_inph.wl(), _output_quad.iwl());
        data.fc.real(static_cast<SLFixPoint>(_output_inph));
        data.fc.imag(static_cast<SLFixPoint>(_output_quad));
    }

    return _output_ready;
}

void DigitalDownConverter::tick(void)
{

}

bool DigitalDownConverter::push(
        // Inputs
        const SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE>    &input_sample,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> cosine;
    SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> sine;

    if (pull_sinusoid(cosine, sine)) {
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
        // Outputs
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & cosine_out,
        SLFixedPoint<TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & sine_out)
{
    // Constants
    SLFixedPoint<OUTWIDTH, 3, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> FPM_2PI = 2.0 * M_PI;
    // Fixed point local variables
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND>                                dithered_phase_acc;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_TRUNCATE, SLFixPoint::OVERFLOW_WRAP_AROUND>                                phase_corr;
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
    _phase_acc += _phase_increment;

    return output_ready;    
}

bool DigitalDownConverter::push_halfband(
        // Inputs
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_in,
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out)
{
    const size_t HB_LENGTH = _halfband_coeffs.size();
    bool output_ready = false;

    //XXX how can we analytically determine the best accumulator format here
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
        for (size_t i = 0; i < HB_LENGTH; i += 2) {
            inph_accum += _halfband_inph_delays[i] * _halfband_coeffs[i];
            quad_accum += _halfband_quad_delays[i] * _halfband_coeffs[i];
        }
        inph_out = inph_accum;
        quad_out = quad_accum;

        // Indicate that the output is ready
        output_ready = true;
    }

    return output_ready;
}

bool DigitalDownConverter::push_by5(
        // Inputs
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_in,
        const SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_in,
        // Outputs
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & inph_out,
        SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> & quad_out)
{
    const size_t B5_LENGTH = _by5_coeffs.size();
    bool output_ready = false;

    //XXX how can we analytically determine the best accumulator format here
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
            for (size_t i = 0; i < B5_LENGTH; i++) {
                inph_accum += _by5_inph_delays[i] * _by5_coeffs[i];
                quad_accum += _by5_quad_delays[i] * _by5_coeffs[i];
            }
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
