#include <filters/duc.hpp>

#define DUC_NCO_FP_FORMAT               NCO::TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

//Up2 integer width = output int width + halfband coeff int width
#define DUC_UP2_ACCUM_FORMAT            52, 3, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

//Up5 integer width = output int width + by5 coeff int width
#define DUC_UP5_ACCUM_FORMAT            52, 4, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

DigitalUpConverter::DigitalUpConverter(double freq, const std::vector<double> &by2Coeffs, const std::vector<double> &by5Coeffs) :
    FilterChainElement("DDC"),
    _nco(freq),
    _up2_coeffs(),
    _up2_inph_delays(),
    _up2_quad_delays(),
    _up5_coeffs(),
    _up5_inph_delays(),
    _up5_quad_delays(),
    _output_ready(false),
    _output_inph(0.0),
    _output_quad(0.0),
    _got_input(false),
    _inph_in(),
    _quad_in(),
    _iteration(0)
{
    double coeff_accum = 0.0;
    for (size_t i = 0; i < by2Coeffs.size(); i++) {
        coeff_accum += by2Coeffs[i];
    }
    double coeff_scale = (1.0 / coeff_accum);
    _up2_coeffs.resize(by2Coeffs.size());
    _up2_inph_delays.resize(by2Coeffs.size());
    _up2_quad_delays.resize(by2Coeffs.size());
    for (size_t i = 0; i < by2Coeffs.size(); i++) {
        _up2_coeffs[i] = by2Coeffs[i] * coeff_scale * 2;
        _up2_inph_delays[i] = 0.0;
        _up2_quad_delays[i] = 0.0;
    }

    coeff_accum = 0.0;
    for (size_t i = 0; i < by5Coeffs.size(); i++) {
        coeff_accum += by5Coeffs[i];
    }
    coeff_scale = (1.0 / coeff_accum);
    _up5_coeffs.resize(by5Coeffs.size());
    _up5_inph_delays.resize(by5Coeffs.size());
    _up5_quad_delays.resize(by5Coeffs.size());
    for (size_t i = 0; i < by5Coeffs.size(); i++) {
        _up5_coeffs[i] = by5Coeffs[i] * coeff_scale * 5;
        _up5_inph_delays[i] = 0.0;
        _up5_quad_delays[i] = 0.0;
    }
}

bool DigitalUpConverter::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT);
    _inph_in = data.fc.real();
    _quad_in = data.fc.imag();
    _got_input = true;
    return true;
}

bool DigitalUpConverter::output(filter_io_t &data)
{
    if (_output_ready) {
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(_output_inph.wl(), _output_quad.iwl());
        data.fc.real(static_cast<SLFixPoint>(_output_inph));
        data.fc.imag(static_cast<SLFixPoint>(_output_quad));
    }

    return _output_ready;
}

void DigitalUpConverter::tick(void)
{
    if (_got_input) {
        assert(_iteration == 0);
        _got_input = false;
    } else {
        _inph_in = 0.0;
        _quad_in = 0.0;
    }
    _output_ready = push(_inph_in, _quad_in, _output_inph, _output_quad);
    ++_iteration;
    if (_iteration == 10) {
        _iteration = 0;
    }
}

bool DigitalUpConverter::push(
        // Inputs
        const SLFixedPoint<DUC_INPUT_FP_FORMAT>    &inph_in,
        const SLFixedPoint<DUC_INPUT_FP_FORMAT>    &quad_in,
        // Outputs
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<DUC_NCO_FP_FORMAT> cosine;
    SLFixedPoint<DUC_NCO_FP_FORMAT> sine;

    SLFixedPoint<DUC_OUTPUT_FP_FORMAT> temp_inph_in = inph_in;
    SLFixedPoint<DUC_OUTPUT_FP_FORMAT> temp_quad_in = quad_in;

    SLFixedPoint<DUC_OUTPUT_FP_FORMAT> up5_inph;
    SLFixedPoint<DUC_OUTPUT_FP_FORMAT> up5_quad;
    push_up5(temp_inph_in, temp_quad_in, up5_inph, up5_quad);

    temp_inph_in = up5_inph;
    temp_quad_in = up5_quad;

    if (_iteration == 0 || _iteration == 5) {
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> up2_inph;
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> up2_quad;
        push_up2(temp_inph_in, temp_quad_in, up2_inph, up2_quad);
        temp_inph_in = up2_inph;
        temp_quad_in = up2_quad;
    }

    _nco.pullNextSample(cosine, sine);
    inph_out = (temp_inph_in * cosine) - (temp_quad_in * sine);
    quad_out = 0.0;
    output_ready = true;

    return output_ready;
}

bool DigitalUpConverter::push_up2(
        // Inputs
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_out)
{
    const size_t U2_LENGTH = _up2_coeffs.size();
    SLFixedPoint<DUC_UP2_ACCUM_FORMAT>  inph_accum = 0.0;
    SLFixedPoint<DUC_UP2_ACCUM_FORMAT>  quad_accum = 0.0;

    // Rotate delay line
    for (int i = U2_LENGTH-1; i > 0; i--) {
        _up2_inph_delays[i] = _up2_inph_delays[i - 1];
        _up2_quad_delays[i] = _up2_quad_delays[i - 1];
    }
    _up2_inph_delays[0] = inph_in;
    _up2_quad_delays[0] = quad_in;

    for (size_t i = 0; i < U2_LENGTH; i++) {
        inph_accum += _up2_inph_delays[i] * _up2_coeffs[i];
        quad_accum += _up2_quad_delays[i] * _up2_coeffs[i];
    }
    inph_out = inph_accum;
    quad_out = quad_accum;
    return true;
}

bool DigitalUpConverter::push_up5(
        // Inputs
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_in,
        const SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_in,
        // Outputs
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_OUTPUT_FP_FORMAT> & quad_out)
{
    const size_t U5_LENGTH = _up5_coeffs.size();

    SLFixedPoint<DUC_UP5_ACCUM_FORMAT>  inph_accum = 0.0;
    SLFixedPoint<DUC_UP5_ACCUM_FORMAT>  quad_accum = 0.0;

    // Rotate delay line
    for (int i = U5_LENGTH-1; i > 0; i--) {
        _up5_inph_delays[i] = _up5_inph_delays[i-1];
        _up5_quad_delays[i] = _up5_quad_delays[i-1];
    }
    _up5_inph_delays[0] = inph_in;
    _up5_quad_delays[0] = quad_in;

    for (size_t i = 0; i < U5_LENGTH; i++) {
        inph_accum += _up5_inph_delays[i] * _up5_coeffs[i];
        quad_accum += _up5_quad_delays[i] * _up5_coeffs[i];
    }
    inph_out = inph_accum;
    quad_out = quad_accum;

    return true;
}

