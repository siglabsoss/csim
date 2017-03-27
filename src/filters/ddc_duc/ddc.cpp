#include <filters/ddc_duc/ddc.hpp>

DigitalDownConverter::DigitalDownConverter(double freq, const std::vector<double> &halfbandCoeffs, const std::vector<double> &by5Coeffs) :
    FilterChainElement("DDC"),
    _nco(freq),
    _halfbandFIR(nullptr),
    _by5FIR(nullptr),
    _input(0.0),
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

    FixedFIRConfig hbConf = {
        .wlCoeff        = 18,
        .wlDelay        = DDC_OUTPUT_WL,
        .iwlDelay       =  DDC_OUTPUT_IWL,
        .wlOut          = DDC_OUTPUT_WL,
        .iwlOut         =  DDC_OUTPUT_IWL,
        .rateChange     = -2
    };
    FixedFIRConfig by5Conf = {
        .wlCoeff        = 18,
        .wlDelay        = 18,
        .iwlDelay       =  1,
        .wlOut          = DDC_OUTPUT_WL,
        .iwlOut         =  DDC_OUTPUT_IWL,
        .rateChange     = -5
    };
    _halfbandFIR = new FixedFirRealCoeff(halfbandNormCoeffs, hbConf);
    _by5FIR = new FixedFirRealCoeff(by5NormCoeffs, by5Conf);
}

bool DigitalDownConverter::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT || data.type == IO_TYPE_COMPLEX_DOUBLE);
    _input = data.toComplexDouble().real();
    _did_receive_input = true;
    return true;
}

bool DigitalDownConverter::output(filter_io_t &data)
{
    if (_output_ready) {
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(_output_inph.getFormat());
        //The output of the DUC is Re{x_bb(t) * exp(1j*w_c*t)} = x_bb(t) * cos(w_c*t) = 0.5 * x_bb(t) * [exp(j*w_c*t) + exp(-j*w_c*t)], thus we add a 2x gain here to compensate
        data.fc.real(_output_inph.to_double() * 2);
        data.fc.imag(_output_quad.to_double() * 2);
    }

    return _output_ready;
}

void DigitalDownConverter::tick(void)
{
    //This logic shouldn't be needed because the down converter should be receiving a sample
    //on every tick, and we need to call push on every tick so that the NCO is not jittering.
    //If we really didn't receive a sample, then clear the input and call push anyway.
    //This may happen if there's an upstream block doing some kind of filtering.
    if (!_did_receive_input) {
        _input = 0.0;
        _did_receive_input = false;
    }
    _output_ready = push(_input, _output_inph, _output_quad);
}

bool DigitalDownConverter::push(
        // Inputs
        const SLFixedPoint<DDC_INPUT_FP_FORMAT>    &input_sample,
        // Outputs
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & inph_out,
        SLFixedPoint<DDC_OUTPUT_FP_FORMAT> & quad_out)
{
    bool output_ready = false;
    SLFixedPoint<NCO_FP_FORMAT> cosine;
    SLFixedPoint<NCO_FP_FORMAT> sine;

    _nco.pullNextSample(cosine, sine);

    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> bb_inph;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> bb_quad;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> half_inph;
    SLFixedPoint<DDC_OUTPUT_FP_FORMAT> half_quad;

    bb_inph = input_sample * cosine;
    bb_quad = input_sample * sine;

    filter_io_t sample;
    sample.type = IO_TYPE_COMPLEX_FIXPOINT;
    sample.fc.setFormat(bb_inph.getFormat());
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

//    std::cout << "ddc in: " << input_sample.to_double() << " * " << cosine.to_double() << std::endl;

    return output_ready;
}
