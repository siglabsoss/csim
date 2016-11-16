#include <filters/ddc_duc/duc.hpp>

DigitalUpConverter::DigitalUpConverter(double freq, const std::vector<double> &up2Coeffs, const std::vector<double> &up5Coeffs) :
    FilterChainElement("DDC"),
    _nco(freq),
    _up2FIR(nullptr),
    _up5FIR(nullptr),
    _output_ready(false),
    _output_inph(0.0),
    _output_quad(0.0),
    _got_input(false),
    _inph_in(),
    _quad_in(),
    _iteration(0)
{
    std::vector<double> up2NormCoeffs = up2Coeffs;
    std::vector<double> up5NormCoeffs = up5Coeffs;
    double coeff_accum = 0.0;
    for (size_t i = 0; i < up2Coeffs.size(); i++) {
        coeff_accum += up2Coeffs[i];
    }
    double coeff_scale = (1.0 / coeff_accum);

    for (size_t i = 0; i < up2Coeffs.size(); i++) {
        up2NormCoeffs[i] = up2Coeffs[i] * coeff_scale * 2;
    }

    coeff_accum = 0.0;
    for (size_t i = 0; i < up5Coeffs.size(); i++) {
        coeff_accum += up5Coeffs[i];
    }
    coeff_scale = (1.0 / coeff_accum);
    for (size_t i = 0; i < up5Coeffs.size(); i++) {
        up5NormCoeffs[i] = up5Coeffs[i] * coeff_scale * 5;
    }
    FixedFIR::Config up2Conf = {
        .wlCoeff        = 18,
        .wlDelay        = DUC_INPUT_WL,
        .iwlDelay       =  DUC_INPUT_IWL,
        .wlOut          =  DUC_INPUT_WL,
        .iwlOut         =  DUC_INPUT_IWL,
        .rateChange     =  2
    };
    FixedFIR::Config up5Conf = {
        .wlCoeff        = 18,
        .wlDelay        = DUC_INPUT_WL,
        .iwlDelay       =  DUC_INPUT_IWL,
        .wlOut          =  DUC_INPUT_WL,
        .iwlOut         =  DUC_INPUT_IWL,
        .rateChange     =  5
    };
    _up2FIR = new FixedFIR(up2NormCoeffs, up2Conf);
    _up5FIR = new FixedFIR(up5NormCoeffs, up5Conf);

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
        data.fc.setFormat(_output_inph.wl(), _output_inph.iwl());
        data.fc.real(static_cast<SLFixPoint>(_output_inph));
        data.fc.imag(static_cast<SLFixPoint>(_output_quad));
    }

    return _output_ready;
}

void DigitalUpConverter::tick(void)
{
    SLFixedPoint<DUC_INPUT_FP_FORMAT> temp_inph_out;
    SLFixedPoint<DUC_INPUT_FP_FORMAT> temp_quad_out;
    _output_ready = push(_inph_in, _quad_in, temp_inph_out, temp_quad_out);

    if (_got_input) {
        assert(_iteration == 0);
        _got_input = false;
    }

    if (_output_ready) {
        _output_inph = temp_inph_out;
        _output_quad = temp_quad_out;
    }
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
        SLFixedPoint<DUC_INPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DUC_INPUT_FP_FORMAT> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<NCO_FP_FORMAT> cosine;
    SLFixedPoint<NCO_FP_FORMAT> sine;

    filter_io_t sample;
    sample.type = IO_TYPE_COMPLEX_FIXPOINT;
    sample.fc.setFormat(inph_in);
    sample.fc.real(inph_in);
    sample.fc.imag(quad_in);
//    std::cout << sample.fc.real().to_double() << "," << sample.fc.imag().to_double() << ",";
    bool up2_has_input = _got_input;
    bool up5_has_input = false;

    if (_iteration == 0 || _iteration == 5) {
        if (up2_has_input) {
            _up2FIR->input(sample);
        }
        _up2FIR->tick();
        up5_has_input = _up2FIR->output(sample);
        assert(up5_has_input);
    }
    if (up5_has_input) {
        _up5FIR->input(sample);
    }
    _up5FIR->tick();
    assert(_up5FIR->output(sample));
//    std::cout << sample.fc.real().to_double() << "," << sample.fc.imag().to_double() << std::endl;

    _nco.pullNextSample(cosine, sine);
    inph_out = (sample.fc.real() * cosine) - (sample.fc.imag() * sine);
    quad_out = 0.0;
    output_ready = true;

    return output_ready;
}

