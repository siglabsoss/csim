
#include <test/unit_test.hpp>

#include <vector> //For storing parsed data from file
#include <stdlib.h>
#include <utils/utils.hpp>

#include <filters/fft.hpp>


using namespace std;

CSIM_TEST_SUITE_BEGIN(FixedFFT)

template <typename T>
void checkError(const vector<T> &outputs, const vector<T> &answers, double difference);

void checkErrorComplexDouble(const vector<ComplexDouble> &outputs, const vector<ComplexDouble> &answers, double difference);
void runFFTTest(const std::string &infile, const std::string &outfile, size_t outputIWL, bool inverse, double threshold);
void runFFTLoopbackTest(const std::string &infile, double threshold);
void runIFFTLoopbackTest(const std::string &infile, double threshold);
void checkErrorComplexInt (const vector<std::complex<int32_t> > &actual, const vector<std::complex<int32_t> > &expected, uint32_t threshold);

//CSIM_TEST_CASE(FFT_IO_PARITY)
//{
//    constexpr size_t NUM_SAMPLES = 2;
//    FFT fft(NUM_SAMPLES, false);
//    std::vector<filter_io_t> samples(NUM_SAMPLES*4);
//    for (size_t i = 0; i < samples.size(); i++) {
//        samples[i].type = IO_TYPE_COMPLEX_FIXPOINT;
//        samples[i].fc.setFormat(32, 2);
//        samples[i].fc.set(1,1);
//    }
//
//    filter_io_t output;
//    bool isOutput = false;
//    unsigned int count = 0;
//    for (size_t j = 0; j < samples.size(); j++) {
//        BOOST_CHECK(fft.input(samples[j]) == true);
//        fft.tick();
//        if (fft.output(output)) {
//        	count++;
//        	isOutput = true;
//        }
//        BOOST_CHECK_MESSAGE(isOutput == ((j + 1 >= NUM_SAMPLES)), "isOutput = " << isOutput << " but j = " << j );
//        isOutput = false;
//    }
//
//    assert( count == samples.size() - (NUM_SAMPLES - 1) );
//}

CSIM_TEST_CASE(FFT_MATLAB_8PT)
{
    runFFTTest("./data/fft/input/input_8pt.csv", "./data/fft/answers/output_8pt.csv", 4, false, 0.1);
}

CSIM_TEST_CASE(FFT_MATLAB)
{
    runFFTTest("./data/fft/input/fft_input1.csv", "./data/fft/answers/fft_output1.csv", 5, false, 1.5e-3);
}

CSIM_TEST_CASE(IFFT_MATLAB)
{
    runFFTTest("./data/fft/input/ifft_input1.csv", "./data/fft/answers/ifft_output1.csv", -5, true, 2.0e-6);
}

CSIM_TEST_CASE(FFT_IFFT)
{
    runFFTLoopbackTest("./data/fft/input/ifft_input1.csv", 5.0e-5);
}

CSIM_TEST_CASE(IFFT_FFT)
{
    runIFFTLoopbackTest("./data/fft/input/bpsk_rand.csv", 1.0e-6);
}

void runFFTTest(const std::string &infile, const std::string &outfile, size_t outputIWL, bool inverse, double threshold)
{
    string inFile(infile);
    string answersFile(outfile);

    std::vector<ComplexDouble> inputs;
    std::vector<ComplexDouble> answers;
    std::vector<ComplexDouble> outputs;

    inputs = utils::readComplexFromCSV<ComplexDouble>(inFile);
    BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
    answers = utils::readComplexFromCSV<ComplexDouble >(answersFile);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

    int points = inputs.size();
    FFT fft(points, inverse);
    fft.setOutputFormat(FFT_OUTPUT_WL, outputIWL, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    filter_io_t data;
    for (unsigned int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {
            //std::cout << j << ": " << inputs[j] << std::endl;
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(FFT_INPUT_FORMAT);
            data.fc.set(inputs[j].real(), inputs[j].imag());
            //std::cout << inputs[j] << " = " << data << std::endl;
            fft.input(data);
            fft.tick();
            bool didGetOutput = fft.output(data);
            bool lastInput = (i == 1 && j == points - 1);
            if (didGetOutput && !lastInput) {
                outputs.push_back(data.toComplexDouble());
            }//If output is ready
        }//Insert all input
    }//Insert input again to get output

    //This threshold is arbitrary and can be refined based on some kind of analysis
    checkErrorComplexDouble(outputs, answers, threshold);
}

void runIFFTLoopbackTest(const std::string &infile, double threshold)
{
    string inFile(infile);

    std::vector<ComplexDouble>          inputs;
    std::vector< SLFixComplex >         ifftoutputs;
    std::vector< ComplexDouble >        fftoutputs;

    inputs = utils::readComplexFromCSV<ComplexDouble>(inFile);
    BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);

    int points = inputs.size();
    FFT fft(points, false);
    FFT ifft(points, true);
    ifft.setOutputFormat(FFT_OUTPUT_WL, -2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    fft.setOutputFormat(FFT_OUTPUT_WL, 6, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    filter_io_t data;
    for (unsigned int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(FFT_INPUT_FORMAT);
            data.fc.set(inputs[j].real(), inputs[j].imag());
            ifft.input(data);
            ifft.tick();
            bool didGetOutput = ifft.output(data);
            bool lastInput = (i == 1 && j == points - 1);
            if (didGetOutput && !lastInput) {
                ifftoutputs.push_back(data.fc);
            }//If output is ready
        }//Insert all input
    }//Insert input again to get output

    for (unsigned int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(ifftoutputs[j].getFormat());
            data.fc = ifftoutputs[j];
            fft.input(data);
            fft.tick();
            bool didGetOutput = fft.output(data);
            bool lastInput = (i == 1 && j == points - 1);
            if (didGetOutput && !lastInput) {
                fftoutputs.push_back(data.fc.toComplexDouble());
            }//If output is ready
        }
    }

    checkErrorComplexDouble (fftoutputs, inputs, threshold);
}

void runFFTLoopbackTest(const std::string &infile, double threshold)
{
    string inFile(infile);

    std::vector<ComplexDouble>          inputs;
    std::vector< SLFixComplex >         fftoutputs;
    std::vector< ComplexDouble >        ifftoutputs;

    inputs = utils::readComplexFromCSV<ComplexDouble>(inFile);
    BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);

    int points = inputs.size();
    FFT fft(points, false);
    FFT ifft(points, true);
    fft.setOutputFormat(FFT_OUTPUT_WL, 5, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    ifft.setOutputFormat(FFT_OUTPUT_WL, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    filter_io_t data;
    for (unsigned int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(FFT_INPUT_FORMAT);
            data.fc.set(inputs[j].real(), inputs[j].imag());
            fft.input(data);
            fft.tick();
            bool didGetOutput = fft.output(data);
            bool lastInput = (i == 1 && j == points - 1);
            if (didGetOutput && !lastInput) {
                fftoutputs.push_back(data.fc);
            }//If output is ready
        }//Insert all input
    }//Insert input again to get output

    for (unsigned int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(fftoutputs[j].getFormat());
            data.fc = fftoutputs[j];
            ifft.input(data);
            ifft.tick();
            bool didGetOutput = ifft.output(data);
            bool lastInput = (i == 1 && j == points - 1);
            if (didGetOutput && !lastInput) {
                ifftoutputs.push_back(data.fc.toComplexDouble());
            }//If output is ready
        }
    }

    checkErrorComplexDouble (ifftoutputs, inputs, threshold);
}

template <typename T>
void checkError(const vector<T> &outputs, const vector<T> &answers, double difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
            double outputReal = outputs[i].real().to_double();
            double outputImag = outputs[i].imag().to_double();
            double answersReal = answers[i].real().to_double();
            double answersImag = answers[i].imag().to_double();
            double realDiff = fabs(outputReal - answersReal);
            double imagDiff = fabs(outputImag - answersImag);
            BOOST_REQUIRE_MESSAGE(realDiff < difference,
            "I: " << i << " Real Actual: " << outputReal << " Real Expected: " << answersReal << " Real Diff: " << realDiff );
            BOOST_REQUIRE_MESSAGE(imagDiff < difference,
            "I: " << i << " Imag Actual: " << outputImag << " Imag Expected: " << answersImag << " Imag Diff: " << imagDiff );
		}
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

void checkErrorComplexInt (const vector<std::complex<int32_t> > &actual, const vector<std::complex<int32_t> > &expected, uint32_t threshold)
{
    unsigned long long accumRealDiff = 0;
    unsigned long long accumImagDiff = 0;
    for (size_t i = 0; i < actual.size(); i++) {
        uint32_t realDiff = fabs(actual[i].real() - expected[i].real());
        uint32_t imagDiff = fabs(actual[i].imag() - expected[i].imag());
        accumRealDiff += realDiff;
        accumImagDiff += imagDiff;

        BOOST_CHECK_MESSAGE(realDiff < threshold, "Real Actual: " << actual[i].real() << " Real Expected: " << expected[i].real() << " Real Diff: " << realDiff);
        BOOST_CHECK_MESSAGE(imagDiff < threshold, "Imag Actual: " << actual[i].imag() << " Imag Expected: " << expected[i].imag() << " Imag Diff: " << imagDiff);
    }

    double avgRealDiff = static_cast<double>(accumRealDiff) / actual.size();
    double avgImagDiff = static_cast<double>(accumImagDiff) / actual.size();

    std::cout << "avgRealDiff = " << avgRealDiff << " avgImagDiff = " << avgImagDiff << std::endl;
}

void checkErrorComplexDouble(const vector<ComplexDouble> &outputs, const vector<ComplexDouble> &answers, double difference)
{
    for (unsigned int i = 0; i < answers.size(); i++) {
            double outputReal = outputs[i].real();
            double outputImag = outputs[i].imag();
            double answersReal = answers[i].real();
            double answersImag = answers[i].imag();
            double realDiff = fabs(outputReal - answersReal);
            double imagDiff = fabs(outputImag - answersImag);
            BOOST_CHECK_MESSAGE(realDiff < difference,
            "I: " << i << " Real Actual: " << outputReal << " Real Expected: " << answersReal << " Real Diff: " << realDiff );
            BOOST_CHECK_MESSAGE(imagDiff < difference,
            "I: " << i << " Imag Actual: " << outputImag << " Imag Expected: " << answersImag << " Imag Diff: " << imagDiff );
        }
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

CSIM_TEST_SUITE_END()
