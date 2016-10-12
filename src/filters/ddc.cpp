#include <filters/ddc.hpp>

#define DDC_NCO_FP_FORMAT               NCO::TBWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

DigitalDownConverter::DigitalDownConverter(double freq, const std::vector<double> &halfbandCoeffs, const std::vector<double> &by5Coeffs) :
    FilterChainElement("DDC"),
    _nco(freq),
    _halfbandFIR(nullptr),
    _by5FIR(nullptr),
    _output_ready(false),
    _output_inph(0.0),
    _output_quad(0.0)
{
    std::vector<double> halfbandNormCoeffs = halfbandCoeffs;
    std::vector<double> by5NormCoeffs = by5Coeffs;

    double coeff_accum = 0.0;
    for (size_t i = 0; i < halfbandCoeffs.size(); i++) {
        coeff_accum += halfbandCoeffs[i];
    }
    double coeff_scale = (1.0 / coeff_accum);
    for (size_t i = 0; i < halfbandCoeffs.size(); i++) {
        halfbandNormCoeffs[i] *= coeff_scale;
    }
    coeff_accum = 0.0;
    for (size_t i = 0; i < by5Coeffs.size(); i++) {
        coeff_accum += by5Coeffs[i];
    }
    coeff_scale = (1.0 / coeff_accum);
    for (size_t i = 0; i < by5Coeffs.size(); i++) {
        by5NormCoeffs[i] *= coeff_scale;
    }

    FixedFIR::Config hbConf = {
        .wlCoeff        = 18,
        .wlDelay        = 18,
        .iwlDelay       =  1,
        .wlOut          = 18,
        .iwlOut         =  1,
        .rateChange     = -2
    };
    FixedFIR::Config by5Conf = {
        .wlCoeff        = 18,
        .wlDelay        = 18,
        .iwlDelay       =  1,
        .wlOut          = 18,
        .iwlOut         =  1,
        .rateChange     = -5
    };
    _halfbandFIR = new FixedFIR(halfbandNormCoeffs, hbConf);
    _by5FIR = new FixedFIR(by5NormCoeffs, by5Conf);
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
        data.fc.setFormat(_output_inph.wl(), _output_inph.iwl());
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

    filter_io_t sample;
    sample.type = IO_TYPE_COMPLEX_FIXPOINT;
    sample.fc.setFormat(bb_inph);
    sample.fc.real(bb_inph);
    sample.fc.imag(bb_quad);

    _halfbandFIR->input(sample);
    _halfbandFIR->tick();
    if (_halfbandFIR->output(sample)) {
        _by5FIR->input(sample);
        _by5FIR->tick();
        if (_by5FIR->output(sample)) {
            inph_out = sample.fc.real();
            quad_out = sample.fc.imag();
            output_ready = true;
        }
    }

    return output_ready;
}
