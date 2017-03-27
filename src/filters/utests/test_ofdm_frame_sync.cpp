#include <test/unit_test.hpp>

#include <filters/ofdm_frame_sync.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <utils/utils.hpp>
#include <iomanip>


static void runSamplesThrough(const std::string& inputFile, OFDMFrameSync& fs)
{
    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(
        inputFile);

    filter_io_t sample;

    for (size_t i = 0; i < inputs.size(); ++i) {
        sample.type = IO_TYPE_COMPLEX_FIXPOINT;
        sample.fc.setFormat(DDC_OUTPUT_FP_FORMAT);
        sample.fc.real(inputs[i].real());
        sample.fc.imag(inputs[i].imag());

        fs.input(sample);

        for (size_t j = 0; j < 10; ++j) {
            fs.tick();
            fs.output(sample);
        }
    }
}

CSIM_TEST_SUITE_BEGIN(validateOFDMFrameSync)


// CSIM_TEST_CASE(DID_DETECT_CORRECT_NUMBER_OF_FRAMES_HIGH_SNR)
// {
//     constexpr size_t symbolLen = 1024;
//     constexpr size_t cpLen     = 100;
//
//     MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, 3888, symbolLen);
//     OFDMFrameSync fs(cpLen, symbolLen / 2, mcs);
//
//
//
//
//
//
// runSamplesThrough("./data/frame_sync/ofdm_frame_captures/10_frames_60db.csv",
//                       fs);
//
//     // The sample input has the start of the 11th frame. Enough to detect
//     // an extra peak, but not enough to fully detect the 10th frame
//     BOOST_CHECK_EQUAL(fs.getPeakDetectionCount(),  21);
//     BOOST_CHECK_EQUAL(fs.getFrameDetectionCount(), 9);
// }

CSIM_TEST_CASE(DID_DETECT_CORRECT_NUMBER_OF_FRAMES_LOW_SNR)
{
    constexpr size_t symbolLen = 1024;
    constexpr size_t cpLen     = 100;

    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, 3888, symbolLen);
    OFDMFrameSync fs(cpLen, symbolLen / 2, mcs);

    runSamplesThrough("./data/frame_sync/ofdm_frame_captures/10_frames_0db.csv",
                      fs);

    BOOST_CHECK_EQUAL(fs.getPeakDetectionCount(),  21);
    BOOST_CHECK_EQUAL(fs.getFrameDetectionCount(), 9);
}

CSIM_TEST_SUITE_END()
