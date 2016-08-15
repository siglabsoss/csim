#include <test/unit_test.hpp>

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include <utils/utils.hpp> //reverseBits()
#include <filters/fixedifft.hpp>
#include <filters/fixedfft.hpp>

#include <cfloat>

using namespace std;


CSIM_TEST_SUITE_BEGIN(FixedIFFT)

CSIM_TEST_CASE(IFFT_CONSTANT_OUTPUT)
{
    constexpr size_t NUM_SAMPLES = 8;
    fixedifft ifft(NUM_SAMPLES);
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
                BOOST_CHECK_CLOSE(output.fcn32.real().to_double(), 1.0 / NUM_SAMPLES, DBL_EPSILON);
                BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(), 1.0 / NUM_SAMPLES, DBL_EPSILON);
            }
        }
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
	fixedifft ifft(points,2949120 ); //x point fft, y table size
	filter_io_t data;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = inputs[j];
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				outputs.push_back(data.fcn32);
			}//If output is ready
		}//Insert all input
	}//Insert input again to get output

	assert(answers.size() == outputs.size());

	vector<FixedComplex32> temp(outputs.size());
	for (int i = 0; i < outputs.size(); i++) {
		temp[reverseBits(inputs.size(), i)] = outputs[i];
	}//Reformats data in correct order


	for (int i = 0; i < answers.size(); i++) {
	    double ratioReal = abs((temp[i].real() - answers[i].real())/answers[i].real());
	    double ratioImag = abs((answers[i].imag() - temp[i].imag() )/answers[i].imag());
	    double realDiff = abs(temp[i].real() - answers[i].real());
	    double imagDiff = abs(temp[i].imag() - answers[i].imag());
		BOOST_CHECK_MESSAGE(ratioReal < .01 || realDiff < 3 / 32768.0 ,
		"I: " << i << " Output: " << temp[i].real() << " Answer: " << answers[i].real() << " Ratio: " << ratioReal );
		BOOST_CHECK_MESSAGE(ratioImag < .01 || imagDiff < 3 / 32768.0,
		"I: " << i << " Output: " << temp[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << ratioImag );
	}
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
    fixedifft ifft(points); //x point fft, y table size

	for (int j = 0; j < points; j++) {

		data = inputs[j];
		ifft.input(data);
		bool test = ifft.output(data);
		if (test) {
		   outputs.push_back(data.fcn32);
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
        	outputs.push_back(data.fcn32);
        }
    }//second set of data, gets first set of answers out
    assert(answers.size() == outputs.size());
    std::vector<FixedComplex32> temp(outputs.size());
    for (int i = 0; i < outputs.size(); i++) {
        temp[reverseBits(outputs.size(), i)] = outputs[i];
    }//Reformats data in correct order


    for (int i = 0; i < answers.size(); i++) {
        double realRatio = abs((temp[i].real() - answers[i].real())/answers[i].real());
        double imagRatio = abs((answers[i].imag() - temp[i].imag() )/answers[i].imag());
        double realDiff  = abs(temp[i].real() - answers[i].real());
        double imagDiff  = abs(temp[i].imag() - answers[i].imag());
        BOOST_CHECK_MESSAGE(realRatio < .03 || realDiff < 1 / 32768.0 ,
                "I: " << i << " Output: " << temp[i].real() << " Answer: " << answers[i].real() << " Ratio: " << realRatio );
        BOOST_CHECK_MESSAGE(imagRatio < .03 || imagDiff < 1 / 32768.0,
                "I: " << i << " Output: " << temp[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << imagRatio );

	}//Ensures output and answers are reasonably close for first set of data

    answers.clear();
	answers = complexRead32Scaled(answersFile2);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile2);
	outputs.clear();

    for (int j = 0; j < points; j++) {

		data = FixedComplex32(0,0);
		ifft.input(data);
		bool test = ifft.output(data);
		if (test) {
			outputs.push_back(data.fcn32);
		}// if output is ready
	}//Get 2nd set of data out

    std::vector<FixedComplex32> temp2(outputs.size());
    assert(answers.size() == outputs.size());
    for (int i = 0; i < answers.size(); i++) {
        temp2[reverseBits(outputs.size(), i)] = outputs[i];
    }//Reformats data in correct order

    for (int i = 0; i < answers.size(); i++) {
        double realRatio = abs((temp2[i].real() - answers[i].real())/answers[i].real());
        double imagRatio = abs((answers[i].imag() - temp2[i].imag() )/answers[i].imag());
        double realDiff = abs(temp2[i].real() - answers[i].real());
        double imagDiff = abs(temp2[i].imag() - answers[i].imag());
        BOOST_CHECK_MESSAGE(realRatio < .01 || realDiff < 4 / 32768.0,
           "I: " << i << " Output: " << temp2[i].real() << " Answer: " << answers[i].real() << " Ratio: " << realDiff );
        BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 4 / 32768.0,
           "I: " << i << " Output: " << temp2[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << imagDiff );
    }
}//Checks for two consecutive sets of inputs in the same IFFT.


CSIM_TEST_CASE(FFT_IFFT)
{
	string inFile("../data/fft/input/data_file_complex1.csv");
	vector<FixedComplex32> inputs;
	vector<FixedComplex32> outputs;
	int i = 0;
	inputs = complexRead32Scaled(inFile);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);

	std::vector<FixedComplex32> answers;

	int points = inputs.size();
	filter_io_t data;
	fixedfft fft(points);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = inputs[j];
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				outputs.push_back(data.fcn32);
			}
		}
	}//Take fft of data

    std::vector<FixedComplex32> temp(outputs.size());
	for (i = 0; i < inputs.size(); i++) {
		temp[reverseBits(inputs.size(), i)] = outputs[i];
	}//Reformats data in correct order

	points = outputs.size();

	fixedifft ifft(points);
	outputs.clear();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {

			data = temp[j];
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				outputs.push_back(data.fcn32);
			}
		}
	}//Take ifft of output from fft

	for (i = 0; i < outputs.size(); i++) {
		temp[reverseBits(outputs.size(), i)] = outputs[i];
	}//Reformats data from bit reversed format to the correct order

	assert(inputs.size() == outputs.size());
	for (i = 0; i < inputs.size(); i++) {
        double realRatio = abs((temp[i].real() - inputs[i].real())/inputs[i].real());
        double imagRatio = abs((inputs[i].imag() - temp[i].imag() )/inputs[i].imag());
        double realDiff = abs(temp[i].real() - inputs[i].real());
        double imagDiff = abs(temp[i].imag() - inputs[i].imag());
		BOOST_CHECK_MESSAGE(realRatio <  .01 || realDiff < 5 / 32768.0,
				inputs[i].real() << " Ratio: " << realRatio );
		BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 5 / 32768.0,
		"I: " << i << " Output: " << temp[i].imag() << " Answer: " << inputs[i].imag() << " Ratio: " << abs((temp[i].imag() - inputs[i].imag())/(float)inputs[i].imag()) );
	}//output of IFFT should be the same as original input
}//Used for Running FFT and then IFFT

CSIM_TEST_SUITE_END()

