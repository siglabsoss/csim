#include <test/unit_test.hpp>

#include <vector> //For storing parsed data from file
#include <stdlib.h>
#include <utils/utils.hpp> //reverseBits()
#include <filters/bfp_fft.hpp>

using namespace std;

CSIM_TEST_SUITE_BEGIN(FixedFFT)

template <typename T>
void checkError(const vector<T> &outputs, const vector<T> &answers, double difference);

void checkErrorComplexDouble(const vector<ComplexDouble> &outputs, const vector<ComplexDouble> &answers, double difference);
void runFFTTest(const std::string &infile, const std::string &outfile, bool inverse);
//void runFFTLoopbackTest(const std::string &infile);
void checkErrorComplexInt (const vector<std::complex<int32_t> > &actual, const vector<std::complex<int32_t> > &expected, uint32_t threshold);

CSIM_TEST_CASE(FFT_CONSTANT_INPUTS)
{
    constexpr size_t NUM_SAMPLES = 8;
    constexpr size_t NUM_SAMPLE_SETS = 10;
    filter_io_t data, output;
    data.type = IO_TYPE_COMPLEX_FIXPOINT;
    data.fc.setFormat(32, 2);
    data.fc.set(M_SQRT1_2/3, M_SQRT1_2/3);
    BFPFFT fft(NUM_SAMPLES, false);
    size_t outputCount = 0;
    size_t noOutputCount = 0;

    for (unsigned int i = 0; i < NUM_SAMPLE_SETS; i++) {
        for (unsigned int j = 0; j < NUM_SAMPLES; j++) {
            fft.input(data);
            fft.tick();
            if (fft.output(output)) {
                //double outputReal = output.toComplexDouble().real();
                //double outputImag = output.toComplexDouble().imag();
                double outputReal = output.fc.real().to_double();
                double outputImag = output.fc.imag().to_double();

                double realDiff = 1.0;
                double imagDiff = 1.0;
                double answersReal = 0.0;
                double answersImag = 0.0;

                if (outputCount % NUM_SAMPLES == 0) {
                    answersReal = NUM_SAMPLES * M_SQRT1_2 / 3;
                    answersImag = NUM_SAMPLES * M_SQRT1_2 / 3;
                    realDiff = fabs(outputReal - answersReal);
                    imagDiff = fabs(outputImag - answersImag);
                } else {
                    realDiff = fabs(outputReal - answersReal);
                    imagDiff = fabs(outputImag - answersImag);
                }
                double difference = 0.001;
                BOOST_CHECK_MESSAGE(realDiff < difference,
                "I: " << i << " Real Output: " << outputReal << " Real Answer: " << answersReal << " Real Diff: " << realDiff );
                BOOST_CHECK_MESSAGE(imagDiff < difference,
                "I: " << i << " Imag Output: " << outputImag << " Imag Answer: " << answersImag << " Imag Diff: " << imagDiff );
                //std::cout << outputCount << ": " << output << " " << output.toComplexDouble() << std::endl;
                outputCount++;
            } else {
               noOutputCount++;
            }
        }
    }
    //std::cout << "No outputs for " << noOutputCount << " counts" << std::endl;
    BOOST_CHECK_EQUAL(outputCount, NUM_SAMPLES * (NUM_SAMPLE_SETS - 1) + 1 );
}

CSIM_TEST_CASE(FFT_IO_PARITY)
{
    constexpr size_t NUM_SAMPLES = 2;
    BFPFFT fft(NUM_SAMPLES, false);
    std::vector<filter_io_t> samples(NUM_SAMPLES*4);
    for (size_t i = 0; i < samples.size(); i++) {
        samples[i].type = IO_TYPE_COMPLEX_FIXPOINT;
        samples[i].fc.setFormat(32, 2);
        samples[i].fc.set(1,1);
    }

    filter_io_t output;
    bool isOutput = false;
    unsigned int count = 0;
    for (size_t j = 0; j < samples.size(); j++) {
        BOOST_CHECK(fft.input(samples[j]) == true);
        fft.tick();
        if (fft.output(output)) {
        	count++;
        	isOutput = true;
        }
        BOOST_CHECK_MESSAGE(isOutput == ((j + 1 >= NUM_SAMPLES)), "isOutput = " << isOutput << " but j = " << j );
        isOutput = false;
    }

    assert( count == samples.size() - (NUM_SAMPLES - 1) );
}

CSIM_TEST_CASE(FFT_MATLAB)
{
    runFFTTest("./data/fft/input/fft_input1.csv", "./data/fft/answers/fft_output1.csv", false);
}

CSIM_TEST_CASE(IFFT_MATLAB)
{
    runFFTTest("./data/fft/input/ifft_input1.csv", "./data/fft/answers/ifft_output1.csv", true);
}

//CSIM_TEST_CASE(FFT_IFFT)
//{
//    runFFTLoopbackTest("./data/fft/input/ifft_input1.csv");
//}

void runFFTTest(const std::string &infile, const std::string &outfile, bool inverse)
{
    string inFile(infile);
    string answersFile(outfile);

    std::vector<ComplexDouble> inputs;
    std::vector<ComplexDouble> answers;
    std::vector<ComplexDouble> outputs;

    inputs = readComplexFromCSV<ComplexDouble>(inFile);
    BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
    answers = readComplexFromCSV<ComplexDouble >(answersFile);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

    int points = inputs.size();
    BFPFFT fft(points, inverse);
    filter_io_t data;
    for (unsigned int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {
            //std::cout << j << ": " << inputs[j] << std::endl;
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            data.fc.setFormat(32, 2);
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
    checkErrorComplexDouble(outputs, answers, 0.007);
}

//void runFFTLoopbackTest(const std::string &infile)
//{
//    string inFile(infile);
//
//    std::vector<ComplexDouble> inputs;
//    std::vector<std::complex<int32_t> > inputsInt;
//    std::vector<std::complex<int32_t> > fftoutputs;
//    std::vector<std::complex<int32_t> > ifftoutputs;
//
//    inputs = readComplexFromCSV<ComplexDouble>(inFile);
//    BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
//
//    int points = inputs.size();
//    size_t scaleExponent = utils::calculateInt32ScaleExponent(inputs);
//    BFPFFT fft(points, false);
//    BFPFFT ifft(points, true);
//    filter_io_t data;
//    for (unsigned int i = 0; i < 2; i++) {
//        for (int j = 0; j < points; j++) {
//            //std::cout << j << ": " << inputs[j] << std::endl;
//            inputsInt.push_back(std::complex<int32_t> (inputs[j].real() * static_cast<double>(1u << scaleExponent), inputs[j].imag() * static_cast<double>(1u << scaleExponent)));
//            data.type = IO_TYPE_INT32_COMPLEX;
//            data.intc.c = inputsInt[j];
//            data.intc.exp = 0;
//            //data.intc.real(inputs[j].real() * static_cast<double>(1u << scaleExponent));
//            //data.intc.imag(inputs[j].imag() * static_cast<double>(1u << scaleExponent));
//            fft.input(data);
//            fft.tick();
//            bool didGetOutput = fft.output(data);
//            bool lastInput = (i == 1 && j == points - 1);
//            if (didGetOutput && !lastInput) {
//                fftoutputs.push_back(data.intc.c);
//            }//If output is ready
//        }//Insert all input
//    }//Insert input again to get output
//
//    for (unsigned int i = 0; i < 2; i++) {
//        for (int j = 0; j < points; j++) {
//            data.type = IO_TYPE_INT32_COMPLEX;
//            data.intc.c = fftoutputs[j];
//            ifft.input(data);
//            ifft.tick();
//            bool didGetOutput = ifft.output(data);
//            bool lastInput = (i == 1 && j == points - 1);
//            if (didGetOutput && !lastInput) {
//                //ComplexDouble output(static_cast<double>(data.intc.real()) / static_cast<double>(1ul << outputShiftAmount), static_cast<double>(data.intc.imag()) / static_cast<double>(1ul << outputShiftAmount));
//                int32_t real = data.intc.c.real();
//                int32_t imag = data.intc.c.imag();
//                ssize_t outputShiftAmount = -data.intc.exp;
//
//                if (outputShiftAmount < 0) {
//                    outputShiftAmount = static_cast<ssize_t>(abs(outputShiftAmount));
//                    real <<= outputShiftAmount;
//                    imag <<= outputShiftAmount;
//                }
//                ifftoutputs.push_back(std::complex<int32_t> (real, imag));
//            }//If output is ready
//        }
//    }
//    checkErrorComplexInt (ifftoutputs, inputsInt, (8192)*43);
//}

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
            "I: " << i << " Real Output: " << outputReal << " Real Answer: " << answersReal << " Real Diff: " << realDiff );
            BOOST_REQUIRE_MESSAGE(imagDiff < difference,
            "I: " << i << " Imag Output: " << outputImag << " Imag Answer: " << answersImag << " Imag Diff: " << imagDiff );
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

        BOOST_CHECK_MESSAGE(realDiff < threshold, "Real Output: " << actual[i].real() << " Real Answer: " << expected[i].real() << " Real Diff: " << realDiff);
        BOOST_CHECK_MESSAGE(imagDiff < threshold, "Imag Output: " << actual[i].imag() << " Imag Answer: " << expected[i].imag() << " Imag Diff: " << imagDiff);
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
            "I: " << i << " Real Output: " << outputReal << " Real Answer: " << answersReal << " Real Diff: " << realDiff );
            BOOST_CHECK_MESSAGE(imagDiff < difference,
            "I: " << i << " Imag Output: " << outputImag << " Imag Answer: " << answersImag << " Imag Diff: " << imagDiff );
        }
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

CSIM_TEST_SUITE_END()

