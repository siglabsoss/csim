#include <test/unit_test.hpp>

#include <filters/ofdm_frame_sync.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <utils/utils.hpp>
#include <iomanip>

CSIM_TEST_SUITE_BEGIN(validateOFDMFrameSync)

// XXX make actual assertions
CSIM_TEST_CASE(DID_DETECT_PILOT_AND_PASS_FRAME)
{
    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(
        "./data/frame_sync/preamble_60db_103symbols.csv");

    std::cout << std::setprecision(52);

    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, 3888, 1024);
    OFDMFrameSync fs(100, mcs);

    filter_io_t sample;

    for (size_t i = 0; i < inputs.size(); ++i) {
        sample.type = IO_TYPE_COMPLEX_FIXPOINT;
        sample.fc.setFormat(DDC_OUTPUT_FP_FORMAT);
        sample.fc.real(inputs[i].real());
        sample.fc.imag(inputs[i].imag());

        // std::cout << i << " - " << inputs[i] << " ---- " << sample <<
        // std::endl;
        fs.input(sample);

        for (size_t j = 0; j < 10; ++j) {
            fs.tick();
            fs.output(sample);
        }
    }
}

CSIM_TEST_SUITE_END()
