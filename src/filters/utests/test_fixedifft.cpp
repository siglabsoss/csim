#include <test/unit_test.hpp>

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <utils/utils.hpp>
#include <filters/fixedfft.hpp>
#include <cfloat>

using namespace std;


CSIM_TEST_SUITE_BEGIN(FixedIFFT)

void checkError(vector<FixedComplex32> outputs, vector<FixedComplex32> answers, float percent, int difference);

CSIM_TEST_CASE(IFFT_CONSTANT_OUTPUT)
{
    constexpr size_t NUM_SAMPLES = 8;
    fixedfft ifft(NUM_SAMPLES, 0,true);
    std::vector<filter_io_t> samples(NUM_SAMPLES);
    samples[0] = FixedComplex32(1.0,1.0);
    for (size_t i = 1; i < samples.size(); i++) {
        samples[i] = FixedComplex32(0.0,0.0);
    }

    filter_io_t output;
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < samples.size(); j++) {
            ifft.input(samples[j]);
            ifft.tick();
            if (ifft.output(output)) {
                BOOST_CHECK_CLOSE(output.fc.real().to_double(), 1.0 / NUM_SAMPLES, DBL_EPSILON);
                BOOST_CHECK_CLOSE(output.fc.imag().to_double(), 1.0 / NUM_SAMPLES, DBL_EPSILON);
            }
        }
    }
}


CSIM_TEST_CASE(IFFT_IO_PARITY)
{
    constexpr size_t NUM_SAMPLES = 8;
    fixedfft ifft(NUM_SAMPLES, 0, true);
    std::vector<filter_io_t> samples(NUM_SAMPLES*10);
    for (size_t i = 0; i < samples.size(); i++) {
        samples[i] = FixedComplex32(1.0,1.0);
    }

    filter_io_t output;
    for (size_t j = 0; j < samples.size(); j++) {
        BOOST_CHECK(ifft.input(samples[j]) == true);
        ifft.tick();
        BOOST_CHECK(ifft.output(output) == (j >= NUM_SAMPLES));
    }
}

CSIM_TEST_CASE(IFFT_OCTAVE)
{
	string inFile("./data/ifft/input/data_file_complex1.csv");
	string answersFile("./data/ifft/answers/answers1.csv");

	vector<FixedComplex32> inputs;
	vector<FixedComplex32> answers;
	vector<FixedComplex32> outputs;

	inputs = complexRead32Scaled(inFile);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
	answers = complexRead32Scaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

	int points = inputs.size();
	fixedfft ifft(points,2949120, true ); //x point fft, y table size
	filter_io_t data;
	for (unsigned int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = inputs[j];
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				outputs.push_back(data.fc);
			}//If output is ready
		}//Insert all input
	}//Insert input again to get output

	assert(answers.size() == outputs.size());

	vector<FixedComplex32> temp(outputs.size());
	for (unsigned int i = 0; i < outputs.size(); i++) {
		temp[reverseBits(inputs.size(), i)] = outputs[i];
	}//Reformats data in correct order

	checkError(temp, answers, .01, 3);
}



CSIM_TEST_CASE(IFFT_TWO_INPUTS)
{
    string inFile("../data/ifft/input/data_file_complex2.csv");
    string inFile2("../data/ifft/input/data_file_complex3.csv");
    string answersFile("../data/ifft/answers/answers2.csv");
    string answersFile2("../data/ifft/answers/answers3.csv");

    vector<FixedComplex32> inputs;
    vector<FixedComplex32> answers;
    vector<FixedComplex32> outputs; //Array to store answers

    inputs = complexRead32Scaled(inFile);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
	answers = complexRead32Scaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

    int points = inputs.size();
    filter_io_t data;
    fixedfft ifft(points,0,true); //x point fft, y table size

	for (int j = 0; j < points; j++) {

		data = inputs[j];
		ifft.input(data);
		bool test = ifft.output(data);
		if (test) {
		   outputs.push_back(data.fc);
		}//If output is ready
	}//Insert first set of data

    inputs.clear();

    inputs = complexRead32Scaled(inFile2);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile2);

    for (int j = 0; j < points; j++) {

        data = inputs[j];
        ifft.input(data);
        bool test = ifft.output(data);
        if (test) {
        	outputs.push_back(data.fc);
        }
    }//second set of data, gets first set of answers out
    assert(answers.size() == outputs.size());
    std::vector<FixedComplex32> temp(outputs.size());
    for (unsigned int i = 0; i < outputs.size(); i++) {
        temp[reverseBits(outputs.size(), i)] = outputs[i];
    }//Reformats data in correct order

    checkError(temp, answers, .03, 5);

    answers.clear();
	answers = complexRead32Scaled(answersFile2);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile2);
	outputs.clear();

    for (int j = 0; j < points; j++) {

		data = FixedComplex32(0,0);
		ifft.input(data);
		bool test = ifft.output(data);
		if (test) {
			outputs.push_back(data.fc);
		}// if output is ready
	}//Get 2nd set of data out

    std::vector<FixedComplex32> temp2(outputs.size());
    assert(answers.size() == outputs.size());
    for (unsigned int i = 0; i < answers.size(); i++) {
        temp2[reverseBits(outputs.size(), i)] = outputs[i];
    }//Reformats data in correct order

    checkError(temp2, answers, .03, 5);
}//Checks for two consecutive sets of inputs in the same IFFT.


CSIM_TEST_CASE(FFT_IFFT)
{
	string inFile("../data/fft/input/data_file_complex1.csv");
	vector<FixedComplex32> inputs;
	vector<FixedComplex32> outputs;
	unsigned int i = 0;
	inputs = complexRead32Scaled(inFile);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);

	std::vector<FixedComplex32> answers;

	int points = inputs.size();
	filter_io_t data;
	fixedfft fft(points);

	for (unsigned int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = inputs[j];
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				outputs.push_back(data.fc);
			}
		}
	}//Take fft of data

    std::vector<FixedComplex32> temp(outputs.size());
	for (i = 0; i < inputs.size(); i++) {
		temp[reverseBits(inputs.size(), i)] = outputs[i];
	}//Reformats data in correct order

	points = outputs.size();

	fixedfft ifft(points, 0, true);
	outputs.clear();
	for (unsigned int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {

			data = temp[j];
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				outputs.push_back(data.fc);
			}
		}
	}//Take ifft of output from fft

	for (i = 0; i < outputs.size(); i++) {
		temp[reverseBits(outputs.size(), i)] = outputs[i];
	}//Reformats data from bit reversed format to the correct order

	assert(inputs.size() == outputs.size());
	checkError(temp, inputs, .01, 5); //output of IFFT should be the same as original input

}//Used for Running FFT and then IFFT


void checkError(vector<FixedComplex32> outputs, vector<FixedComplex32> answers, float percent, int difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
		    double ratioReal = abs((outputs[i].real() - answers[i].real())/answers[i].real());
		    double ratioImag = abs((answers[i].imag() - outputs[i].imag() )/answers[i].imag());
		    double realDiff = abs(outputs[i].real() - answers[i].real());
		    double imagDiff = abs(outputs[i].imag() - answers[i].imag());
			BOOST_CHECK_MESSAGE(ratioReal < percent || realDiff < difference / 32768.0 ,
			"I: " << i << " Output: " << outputs[i].real() << " Answer: " << answers[i].real() << " Ratio: " << ratioReal );
			BOOST_CHECK_MESSAGE(ratioImag < percent || imagDiff < difference / 32768.0,
			"I: " << i << " Output: " << outputs[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << ratioImag );
		}
}//Compares results of ifft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int


CSIM_TEST_SUITE_END()
