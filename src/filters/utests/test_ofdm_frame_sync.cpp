#include <test/unit_test.hpp>

#include <filters/ofdm_frame_sync.hpp>
#include <filters/fft.hpp>
#include <utils/utils.hpp>


CSIM_TEST_SUITE_BEGIN(validateOFDMFrameSync)

CSIM_TEST_CASE(FOO_BAR)
{
    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(
        "./data/frame_sync/preamble_10db_snr.csv");
    std::vector<ComplexDouble> inputs2(inputs.size() + 500, 0.0);

    for (size_t i = 0; i < inputs.size(); ++i) {
        inputs2[500 + i] = inputs[i];
    }

    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM16, 4000, 1024,
            std::vector<bool>(1024, true));
    OFDMFrameSync fs(100, mcs);

    filter_io_t sample;
    sample.type = IO_TYPE_COMPLEX_FIXPOINT;
    sample.fc.setFormat(FFT_INPUT_FORMAT);

    for (size_t i = 0; i < inputs.size(); ++i) {
        sample.fc.real(inputs2[i].real());
        sample.fc.imag(inputs2[i].imag());
        fs.input(sample);
        fs.tick();
        fs.output(sample);
    }
}

CSIM_TEST_SUITE_END()
