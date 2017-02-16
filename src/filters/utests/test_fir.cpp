#include <test/unit_test.hpp>
#include <filters/fixed_fir.hpp>
#include <boost/tokenizer.hpp> // For parsing data from file
#include <utils/utils.hpp>

CSIM_TEST_SUITE_BEGIN(FIRFilter)

static constexpr double ONE_BIT_ERROR = (1.0 / (1 << 15));

void checkError(const std::vector<ComplexDouble>& outputs,
                const std::vector<ComplexDouble>& answers,
                float                             threshold)
{
    for (unsigned int i = 0; i < answers.size(); i++) {
        double realDiff = fabs(outputs[i].real() - answers[i].real());
        double imagDiff = fabs(outputs[i].imag() - answers[i].imag());
        BOOST_CHECK_MESSAGE(realDiff < threshold  || realDiff == 0,
                            "I: " << i << " Output: " << outputs[i].real() <<
                            " Answer: " << answers[i].real());
        BOOST_CHECK_MESSAGE(imagDiff < threshold || imagDiff == 0,
                            "I: " << i << " Output: " << outputs[i].imag() <<
                            " Answer: " << answers[i].imag());
    }
} // Compares results of fft with answers. Takes in vector of outputs and

// answers, the max percent error as a float, and the max difference as an int

void runTest(const std::string& coeffFile,
             const std::string& inputFile,
             const std::string& outputFile,
             float              threshold)
{
    SLFixPoint::throwOnOverflow = true;
    std::vector<ComplexDouble> input;                             // Vector to
                                                                  // hold inputs
    std::vector<ComplexDouble> output;                            // Vector to
                                                                  // hold
                                                                  // outputs
    std::vector<ComplexDouble> answers;                           // Vector to
                                                                  // hold
                                                                  // answers
    std::vector<ComplexDouble> coeffs;                            // Vector to
                                                                  // hold
                                                                  //
                                                                  //
                                                                  // coefficients
    coeffs = utils::readComplexFromCSV<ComplexDouble>(coeffFile); // Reads in
                                                                  // taps from
                                                                  // file
    BOOST_REQUIRE_MESSAGE(!coeffs.empty(), "Could not read from " << coeffFile);

    std::vector<double> realCoeffs;

    for (size_t i = 0; i < coeffs.size(); i++) {
        realCoeffs.push_back(coeffs[i].real());
    }

    input = utils::readComplexFromCSV<ComplexDouble>(inputFile);    // Reads
                                                                    // input
                                                                    // file
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inputFile);

    answers = utils::readComplexFromCSV<ComplexDouble>(outputFile); // Reads
                                                                    // answer
                                                                    // file
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << outputFile);

    FixedFIRConfig conf = {
        .wlCoeff    = 18,
        .wlDelay    = 18,
        .iwlDelay   =  1,
        .wlOut      = 18,
        .iwlOut     =  1,
        .rateChange = 0
    };
    FixedFirRealCoeff fir(realCoeffs, conf);

    for (unsigned int k = 0; k < input.size(); k++) {
        filter_io_t  data;
        SLFixComplex sample;
        sample.setFormat(18,
                         1,
                         SLFixPoint::QUANT_RND_HALF_UP,
                         SLFixPoint::OVERFLOW_SATURATE);
        sample.real(input[k].real());
        sample.imag(input[k].imag());
        data = sample;
        fir.input(data); // Filters data
        fir.tick();
        filter_io_t output_sample;
        bool didOutput = fir.output(output_sample);
        BOOST_REQUIRE_EQUAL(didOutput, true);
        output.push_back(output_sample.fc.toComplexDouble());

        // std::cout << output_sample.toComplexDouble() << std::endl;
    }

    checkError(output, answers, threshold);
}

void runImpulseResponse(const std::string& coeffFile)
{
    SLFixPoint::throwOnOverflow = true;
    std::vector<ComplexDouble> coeffs;                            // Vector to
                                                                  // hold
                                                                  //
                                                                  //
                                                                  // coefficients
    std::vector<ComplexDouble> output;                            // Vector to
                                                                  // hold
                                                                  // outputs

    coeffs = utils::readComplexFromCSV<ComplexDouble>(coeffFile); // Reads in
                                                                  // taps from
                                                                  // file
    BOOST_REQUIRE_MESSAGE(!coeffs.empty(), "Could not read from " << coeffFile);

    std::vector<double> realCoeffs;

    for (size_t i = 0; i < coeffs.size(); i++) {
        realCoeffs.push_back(coeffs[i].real());
    }

    FixedFIRConfig conf = {
        .wlCoeff    = 18,
        .wlDelay    = 18,
        .iwlDelay   =  1,
        .wlOut      = 18,
        .iwlOut     =  1,
        .rateChange = 0
    };

    FixedFirRealCoeff fir(realCoeffs, conf);
    filter_io_t  data;
    SLFixComplex sample;
    sample.setFormat(18,
                     1,
                     SLFixPoint::QUANT_RND_HALF_UP,
                     SLFixPoint::OVERFLOW_SATURATE);

    for (size_t i = 0; i < realCoeffs.size(); i++) {
        if (i == 0) {
            sample.real(0.5);
            sample.imag(0.5);
        } else {
            sample.real(0.0);
            sample.imag(0.0);
        }
        data = sample;
        fir.input(data); // Filters data
        fir.tick();
        filter_io_t output_sample;
        bool didOutput = fir.output(output_sample);
        BOOST_REQUIRE_EQUAL(didOutput, true);
        output.push_back(output_sample.fc.toComplexDouble());
    }

    for (size_t i = 0; i < realCoeffs.size(); i++) {
        double realDiff = fabs(realCoeffs[i] - 2 * output[i].real());
        double imagDiff = fabs(realCoeffs[i] - 2 * output[i].imag());
        BOOST_CHECK(realDiff < 0.00002);
        BOOST_CHECK(imagDiff < 0.00002);
    }
}

CSIM_TEST_CASE(FIR_REAL_ONLY_INPUTS)
{
    runTest("./data/fir/input/taps1.txt",
            "./data/fir/input/data1_in.csv",
            "./data/fir/answers/answers1.csv",
            2 * ONE_BIT_ERROR);
}

CSIM_TEST_CASE(FIR_COMPLEX_INPUTS)
{
    runTest("./data/fir/input/taps2.txt",
            "./data/fir/input/data2_in.csv",
            "./data/fir/answers/answers2.csv",
            2 * ONE_BIT_ERROR);
}

CSIM_TEST_CASE(FIR_IMPULSE_RESPONSE)
{
    runImpulseResponse("./data/fir/input/taps2.txt");
    runImpulseResponse("./data/fir/input/taps3.txt"); // DDC down by 5
                                                      // coefficients
    runImpulseResponse("./data/fir/input/taps4.txt"); // DDC down by 2
                                                      // coefficients
}

CSIM_TEST_SUITE_END()
