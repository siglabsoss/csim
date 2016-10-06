#include <test/unit_test.hpp>
#include <filters/fixed_fir.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <utils/utils.hpp>

CSIM_TEST_SUITE_BEGIN(FIRFilter)

static constexpr double ONE_BIT_ERROR = (1.0 / (1 << 15));

void checkError(const std::vector<ComplexDouble> &outputs, const std::vector<ComplexDouble> &answers, float threshold)
{
    for (unsigned int i = 0; i < answers.size(); i++) {
            double realDiff = fabs(outputs[i].real() - answers[i].real());
            double imagDiff = fabs(outputs[i].imag() - answers[i].imag());
            BOOST_CHECK_MESSAGE(realDiff < threshold  || realDiff == 0,
            "I: " << i << " Output: " << outputs[i].real() << " Answer: " << answers[i].real());
            BOOST_CHECK_MESSAGE(imagDiff < threshold || imagDiff == 0,
            "I: " << i << " Output: " << outputs[i].imag() << " Answer: " << answers[i].imag());
        }
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

void runTest(const std::string &coeffFile, const std::string &inputFile, const std::string &outputFile, float threshold)
{
    std::vector<ComplexDouble> input; //Vector to hold inputs
    std::vector<ComplexDouble> output; //Vector to hold outputs
    std::vector<ComplexDouble> answers; //Vector to hold answers
    std::vector<ComplexDouble>  coeffs; //Vector to hold coefficients
    coeffs = readComplexFromCSV<ComplexDouble>(coeffFile);//Reads in taps from file
    BOOST_REQUIRE_MESSAGE(!coeffs.empty(), "Could not read from " << coeffFile);

    std::vector<double> realCoeffs;
    for (size_t i = 0; i < coeffs.size(); i++) {
        realCoeffs.push_back(coeffs[i].real());
    }

    input = readComplexFromCSV<ComplexDouble>(inputFile); //Reads input file
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inputFile);

    answers = readComplexFromCSV<ComplexDouble>(outputFile); //Reads answer file
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << outputFile);

    FixedFIR fir(realCoeffs, 16);

    for (unsigned int k = 0; k < input.size(); k++) {
        filter_io_t data;
        SLFixComplex sample;
        sample.setFormat(16, 1);
        sample.real(input[k].real());
        sample.imag(input[k].imag());
        data = sample;
        fir.input(data); //Filters data
        filter_io_t output_sample;
        bool didOutput = fir.output(output_sample);
        BOOST_REQUIRE_EQUAL(didOutput, true);
        output.push_back(output_sample.fc.toComplexDouble());
        //std::cout << output_sample.toComplexDouble() << std::endl;
    }

    checkError(output, answers, threshold);
}

CSIM_TEST_CASE(FIR_REAL_ONLY_INPUTS)
{
    runTest("./data/fir/input/taps1.txt", "./data/fir/input/data1_in.csv", "./data/fir/answers/answers1.csv", 2*ONE_BIT_ERROR);

}

CSIM_TEST_CASE(FIR_COMPLEX_INPUTS)
{
    runTest("./data/fir/input/taps2.txt", "./data/fir/input/data2_in.csv", "./data/fir/answers/answers2.csv", 2*ONE_BIT_ERROR);
}

CSIM_TEST_SUITE_END()

