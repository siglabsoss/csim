#include <test/unit_test.hpp>

#include <vector> //For storing parsed data from file
#include <stdlib.h>
#include <utils/utils.hpp> //reverseBits()
#include <filters/bfp_fft.hpp>

using namespace std;

CSIM_TEST_SUITE_BEGIN(FixedFFT)

template <typename T>
void checkError(const vector<T> &outputs, const vector<T> &answers, double difference);

CSIM_TEST_CASE(CONSTANT_INPUTS)
{
    constexpr size_t NUM_SAMPLES = 1024;
    constexpr size_t NUM_SAMPLE_SETS = 10;
    filter_io_t data, output;
    data = FixedComplex32(M_SQRT1_2/3,  M_SQRT1_2/3); //QAM16 0000 symbol
    BFPFFT fft(NUM_SAMPLES);
    size_t outputCount = 0;
    size_t noOutputCount = 0;
    for (unsigned int i = 0; i < NUM_SAMPLE_SETS; i++) {
        for (unsigned int j = 0; j < NUM_SAMPLES; j++) {
            fft.input(data);
            fft.tick();
            if (fft.output(output)) {
                //std::cout << outputCount << ": " << output << " " << output.toComplexDouble() << std::endl;
                if (outputCount % NUM_SAMPLES == 0) {
                    BOOST_CHECK_CLOSE(output.toComplexDouble().real(), NUM_SAMPLES * M_SQRT1_2 / 3, 0.1);
                    BOOST_CHECK_CLOSE(output.toComplexDouble().imag(), NUM_SAMPLES * M_SQRT1_2 / 3, 0.1);
                } else {
                    BOOST_CHECK_CLOSE(output.toComplexDouble().real(), 0.0, 0.1);
                    BOOST_CHECK_CLOSE(output.toComplexDouble().imag(), 0.0, 0.1);
                }
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
    BFPFFT fft(NUM_SAMPLES);
    std::vector<filter_io_t> samples(NUM_SAMPLES*4);
    for (size_t i = 0; i < samples.size(); i++) {
        samples[i] = FixedComplex32(1.0,1.0);
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
  string inFile("./data/fft/input/fft_input1.csv");
  string answersFile("./data/fft/answers/fft_output1.csv");

  std::vector<ComplexDouble> inputs;
  std::vector<ComplexDouble> answers;
  std::vector<ComplexDouble> outputs;

  inputs = complexReadUnscaled<ComplexDouble >(inFile);
  BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
  answers = complexReadUnscaled<ComplexDouble >(answersFile);
  BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

  int points = inputs.size();
  BFPFFT fft(points); //x point fft, y table size
  filter_io_t data;
  for (unsigned int i = 0; i < 2; i++) {
      for (int j = 0; j < points; j++) {
          //std::cout << j << ": " << inputs[j] << std::endl;
          data = inputs[j];
          fft.input(data);
          fft.tick();
          bool test = fft.output(data);
          bool lastInput = (i == 1 && j == points - 1);
          if (test && !lastInput) {
              outputs.push_back(data.rf);
          }//If output is ready
      }//Insert all input
  }//Insert input again to get output

  checkError<ComplexDouble>(outputs, answers, 0.0000001);
}

template <typename T>
void checkError(const vector<T> &outputs, const vector<T> &answers, double difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
		    double realDiff = abs(outputs[i].real() - answers[i].real());
		    double imagDiff = abs(outputs[i].imag() - answers[i].imag());
			BOOST_CHECK_MESSAGE(realDiff < difference,
			"I: " << i << " Real Output: " << outputs[i].real() << " Real Answer: " << answers[i].real() << " Real Diff: " << realDiff );
			BOOST_CHECK_MESSAGE(imagDiff < difference,
			"I: " << i << " Imag Output: " << outputs[i].imag() << " Imag Answer: " << answers[i].imag() << " Imag Ratio: " << imagDiff );
		}
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

CSIM_TEST_SUITE_END()

