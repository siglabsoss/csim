#include <filters/ddc.hpp>

#define DDC_NCO_FP_FORMAT               NCO::TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

//Halfband integer width = output int width + halfband coeff int width
#define DDC_HALFBAND_ACCUM_FORMAT       52,  2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

//By5 integer width = output int width + by5 coeff int width
#define DDC_BY5_ACCUM_FORMAT            52, 1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

DigitalDownConverter::DigitalDownConverter(double freq, const std::vector<double> &halfbandCoeffs, const std::vector<double> &by5Coeffs) :
    FilterChainElement("DDC"),
    _nco(freq),
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
    SLFixedPoint<DDC_INPUT_FP_FORMAT> input = data.fc.real();
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
        const SLFixedPoint<DDC_INPUT_FP_FORMAT>    &input_sample,
        // Outputs
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<DDC_NCO_FP_FORMAT> cosine;
    SLFixedPoint<DDC_NCO_FP_FORMAT> sine;

    _nco.pullNextSample(cosine, sine);

    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> bb_inph;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> bb_quad;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> half_inph;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> half_quad;

    bb_inph = input_sample * cosine;
    bb_quad = input_sample * sine;

    if (push_halfband(bb_inph, bb_quad, half_inph, half_quad)) {
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> by5_inph;
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> by5_quad;

        if (push_by5(half_inph, half_quad, by5_inph, by5_quad)) {
            inph_out = by5_inph;
            quad_out = by5_quad;
            output_ready = true;
        }
    }

    return output_ready;
}

bool DigitalDownConverter::push_halfband(
        // Inputs
        const SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_out)
{
    const size_t HB_LENGTH = _halfband_coeffs.size();
    bool output_ready = false;

    SLFixedPoint<DDC_HALFBAND_ACCUM_FORMAT>  inph_accum = 0.0;
    SLFixedPoint<DDC_HALFBAND_ACCUM_FORMAT>  quad_accum = 0.0;

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
        const SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_out)
{
    const size_t B5_LENGTH = _by5_coeffs.size();
    bool output_ready = false;

    //XXX how can we analytically determine the best accumulator format here
    SLFixedPoint<DDC_BY5_ACCUM_FORMAT>  inph_accum = 0.0;
    SLFixedPoint<DDC_BY5_ACCUM_FORMAT>  quad_accum = 0.0;

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
