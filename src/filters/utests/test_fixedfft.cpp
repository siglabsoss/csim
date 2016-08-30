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
#include <filters/fixedfft.hpp>

using namespace std;


CSIM_TEST_SUITE_BEGIN(FixedFFT)

void checkError(vector<FixedComplex32> outputs, vector<FixedComplex32> answers, float percent, int difference);

CSIM_TEST_CASE(CONSTANT_INPUTS)
{
    constexpr size_t NUM_SAMPLES = 8;
    constexpr size_t NUM_SAMPLE_SETS = 10;
    filter_io_t data, output;
    data = FixedComplex32(M_SQRT1_2/3,  M_SQRT1_2/3); //QAM16 0000 symbol
    fixedfft fft(NUM_SAMPLES);
    size_t outputCount = 0;
    size_t noOutputCount = 0;
    for (unsigned int i = 0; i < NUM_SAMPLE_SETS; i++) {
        for (unsigned int j = 0; j < NUM_SAMPLES; j++) {
            fft.input(data);
            fft.tick();
            if (fft.output(output)) {
                if (outputCount % NUM_SAMPLES == 0) {
                    BOOST_CHECK_CLOSE(output.fc.real().to_double(), 8 * M_SQRT1_2 / 3, 0.1);
                    BOOST_CHECK_CLOSE(output.fc.imag().to_double(), 8 * M_SQRT1_2 / 3, 0.1);
                } else {
                    BOOST_CHECK_CLOSE(output.fc.real().to_double(), 0.0, 0.1);
                    BOOST_CHECK_CLOSE(output.fc.imag().to_double(), 0.0, 0.1);
                }
                outputCount++;
                //std::cout << outputCount << ": " << output << std::endl;
            } else {
               noOutputCount++;
            }
        }
    }
    //std::cout << "No outputs for " << noOutputCount << " counts" << std::endl;
    BOOST_CHECK_EQUAL(outputCount, NUM_SAMPLES * (NUM_SAMPLE_SETS - 1) );
}

CSIM_TEST_CASE(FFT_IO_PARITY)
{
    constexpr size_t NUM_SAMPLES = 2;
    fixedfft fft(NUM_SAMPLES);
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
        BOOST_CHECK(isOutput == ((j >= NUM_SAMPLES)));
        isOutput = false;
    }

    assert(count == samples.size() - NUM_SAMPLES);
}

CSIM_TEST_CASE(FFT_OCTAVE)
{
	string inFile("./data/fft/input/data_file_complex1.csv");
	string answersFile("./data/fft/answers/answers1.csv");

	vector<FixedComplex32> inputs;
	vector<FixedComplex32> answers;
	vector<FixedComplex32> outputs;

	inputs = complexRead32Scaled(inFile);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
	answers = complexRead32Scaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

	int points = inputs.size();
	fixedfft fft(points ); //x point fft, y table size
	filter_io_t data;
	for (unsigned int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = inputs[j];
			fft.input(data);
			bool test = fft.output(data);
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

	assert(answers.size() == temp.size());
	checkError(temp, answers, .20, 5000);
}

CSIM_TEST_CASE(FFT_TWO_INPUTS)
{
    string inFile("./data/fft/input/data_file_complex2.csv");
    string inFile2("./data/fft/input/data_file_complex3.csv");
    string answersFile("./data/fft/answers/answers2.csv");
    string answersFile2("./data/fft/answers/answers3.csv");

    vector<FixedComplex32> inputs;
    vector<FixedComplex32> answers;
    vector<FixedComplex32> outputs; //Array to store answers

    inputs = complexRead32Scaled(inFile);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile);
	answers = complexRead32Scaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile);

    int points = inputs.size();
    filter_io_t data;
    fixedfft fft(points); //x point fft, y table size

	for (int j = 0; j < points; j++) {
		data = inputs[j];
		fft.input(data);
		bool test = fft.output(data);
		if (test) {
		   outputs.push_back(data.fc);
		}//If output is ready
	}//Insert first set of data

    inputs.clear();//Clear out inputs vector

    inputs = complexRead32Scaled(inFile2);
	BOOST_REQUIRE_MESSAGE(!inputs.empty(), "Could not open " << inFile2);

    for (int j = 0; j < points; j++) {

        data = inputs[j];
        fft.input(data);
        bool test = fft.output(data);
        if (test) {
        	outputs.push_back(data.fc);
        }
    }//second set of data, gets first set of answers out
    assert(answers.size() == outputs.size());
    assert(inputs.size() == outputs.size());
    std::vector<FixedComplex32> temp(outputs.size());
    for (unsigned int i = 0; i < outputs.size(); i++) {
        temp[reverseBits(outputs.size(), i)] = outputs[i];
    }//Reformats data in correct order

	checkError(temp, answers, .03, 1);

    answers.clear();
	answers = complexRead32Scaled(answersFile2);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not open " << answersFile2);
	outputs.clear();

    for (int j = 0; j < points; j++) {

		data = FixedComplex32(0,0);
		fft.input(data);
		bool test = fft.output(data);
		if (test) {
			outputs.push_back(data.fc);
		}// if output is ready
	}//Get 2nd set of data out

    std::vector<FixedComplex32> temp2(outputs.size());
    assert(answers.size() == outputs.size());
    for (unsigned int i = 0; i < answers.size(); i++) {
        temp2[reverseBits(outputs.size(), i)] = outputs[i];
    }//Reformats data in correct order

	checkError(temp2, answers, .01, 4);
}//Checks for two consecutive sets of inputs in the same FFT.


//
//CSIM_TEST_CASE(FFT_2_FILES)
//{
//       string infile("../csim/data/fft/output/out1BitReversed.txt");
//       string answersfile("../csim/data/fft/output/answers32768BitReversed.csv");
//
//       int i = 0;
//       FixedComplex32 bitReversedInput[32768];
//       ifstream in(infile.c_str());
//       BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open " << infile);
//       ifstream ans(answersfile.c_str());
//       BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not open " << answersfile);
//
//       std::string token;
//       string line;
//       int inputs = 0;
//       while(getline(in,line)) {
//           istringstream ss(line);
//           getline(ss, token, ',');
//           stringstream strValue;
//           strValue << token;
//           int intValue;
//           strValue >> intValue;
//           bitReversedInput[inputs].real = intValue;
//           getline(ss, token, ',');
//           stringstream strValue2;
//           strValue2 << token;
//           strValue2 >> intValue;
//           bitReversedInput[inputs++].imag = intValue;
//       }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n
//
//       int numAnswers = 0;
//       FixedComplex32 trueAnswers[32768];
//       while(getline(ans,line)) {
//
//         istringstream ss(line);
//         getline(ss, token, ',');
//         stringstream strValue;
//         strValue << token;
//         int intValue;
//         strValue >> intValue;
//         trueAnswers[numAnswers].real = intValue;
//         getline(ss, token, ',');
//         stringstream strValue2;
//         strValue2 << token;
//         strValue2 >> intValue;
//        trueAnswers[numAnswers++].imag = intValue;
//
//     }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n
//
////
////        FixedComplex32 temp[32769];
////        for (i = 0; i < inputs; i++) {
////            temp[i] = bitReversedOutput[reverseBits(inputs, i)];
////        }//Reformats data in correct order
//
//
////       FixedComplex32 temp[32769];
////       for (i = 0; i < inputs; i++) {
////           temp[i] = bitReversedOutput[i];
////       }
//
//
//
//        for (i = 0; i < inputs; i++) {
//            BOOST_CHECK_MESSAGE(abs((bitReversedInput[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) <  .10 || abs(abs(bitReversedInput[i].real) - abs(trueAnswers[i].real)) < 10 ,
//                    "I: " << i << " Input: " << bitReversedInput[i].real << " Answer: " << trueAnswers[i].real << " Ratio: " << abs((bitReversedInput[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
//            BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - bitReversedInput[i].imag )/(float)trueAnswers[i].imag) < .10 || abs(abs(bitReversedInput[i].imag) - abs(trueAnswers[i].imag)) < 10,
//                    "I: " << i << " Input: " << bitReversedInput[i].imag << " Answer: " << trueAnswers[i].imag << " Ratio: " << abs((bitReversedInput[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );
//
//        }
//
//
//}//Used for comparing percent error

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
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

CSIM_TEST_SUITE_END()

