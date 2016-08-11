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

CSIM_TEST_CASE(CONSTANT_INPUTS)
{
    constexpr size_t NUM_SAMPLES = 8;
    constexpr size_t NUM_SAMPLE_SETS = 10;
    filter_io_t data, output;
    data = FixedComplex32(M_SQRT1_2/3,  M_SQRT1_2/3); //QAM16 0000 symbol
    fixedfft fft(NUM_SAMPLES);
    size_t outputCount = 0;
    size_t noOutputCount = 0;
    for (int i = 0; i < NUM_SAMPLE_SETS; i++) {
        for (int j = 0; j < NUM_SAMPLES; j++) {
            fft.input(data);
            fft.tick();
            if (fft.output(output)) {
                if (outputCount % NUM_SAMPLES == 0) {
                    BOOST_CHECK_CLOSE(output.fcn32.real().to_double(), 8 * M_SQRT1_2 / 3, 0.1);
                    BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(), 8 * M_SQRT1_2 / 3, 0.1);
                } else {
                    BOOST_CHECK_CLOSE(output.fcn32.real().to_double(), 0.0, 0.1);
                    BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(), 0.0, 0.1);
                }
                outputCount++;
                //std::cout << outputCount << ": " << output << std::endl;
            } else {
               noOutputCount++;
            }
        }
    }
    //std::cout << "No outputs for " << noOutputCount << " counts" << std::endl;
    BOOST_CHECK_EQUAL(outputCount, NUM_SAMPLE_SETS*NUM_SAMPLES - 2*NUM_SAMPLES + 1);
}

CSIM_TEST_CASE(FFT_OCTAVE)
{
	string infile("./data/fft/input/data_file_complex1.csv");
	string answersfile("./data/fft/answers/answers1.csv");

	int i = 0;
	std::vector<double> realInput(32769);
	std::vector<double> imagInput(32769);
	ifstream in(infile.c_str());
	BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open " << infile);
	ifstream ans(answersfile.c_str());
	BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not open " << answersfile);

	std::string token;
	string line;
	int inputs = 0;
	while(getline(in,line)) {

		istringstream ss(line);
		getline(ss, token, ',');
		stringstream strValue;
		strValue << token;
		int intValue;
		strValue >> intValue;
		realInput[inputs] = intValue;
		getline(ss, token, ',');
		stringstream strValue2;
		strValue2 << token;
		strValue2 >> intValue;
		imagInput[inputs++] = intValue;

	}//Reads in inputs from file. Parsing by commas. Format is: real,imag\n
	int numAnswers = 0;
	std::vector<FixedComplex32> trueAnswers(32768);
	while(getline(ans,line)) {

		istringstream ss(line);
		getline(ss, token, ',');
		stringstream strValue;
		strValue << token;
		int intValue;
		strValue >> intValue;
		trueAnswers[numAnswers].real(intValue / 32768.0);
		getline(ss, token, ',');
		stringstream strValue2;
		strValue2 << token;
		strValue2 >> intValue;
		trueAnswers[numAnswers++].imag(intValue / 32768.0);

	}//Reads in answers from file. Parsing by commas. Format is: real,imag\n


	std::vector<FixedComplex32> answers(32769);
	int count = 0; //How many outputs have been collected

	int points = inputs;
	filter_io_t data;
	fixedfft fft(points,2949120 ); //x point fft, y table size

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {

			data = FixedComplex32(realInput[j] / 32768.0 ,imagInput[j] / 32768.0);
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				answers[count++] = data.fcn32;
			}//If output is ready
		}//Insert all input
	}//Insert input again to get output


	assert(count == inputs);
	// If you want bits to be reversed

	std::vector<FixedComplex32> temp(32769);
	for (i = 0; i < inputs; i++) {
		temp[reverseBits(inputs, i)] = answers[i];
	}//Reformats data in correct order


	for (i = 0; i < inputs; i++) {
	    double ratioReal = abs((temp[i].real() - trueAnswers[i].real())/trueAnswers[i].real());
	    double ratioImag = abs((trueAnswers[i].imag() - temp[i].imag() )/trueAnswers[i].imag());
	    double realDiff = abs(temp[i].real() - trueAnswers[i].real());
	    double imagDiff = abs(temp[i].imag() - trueAnswers[i].imag());
		BOOST_CHECK_MESSAGE(ratioReal < .20 || realDiff < 5000 / 32768.0 ,
		"I: " << i << " Output: " << temp[i].real() << " Answer: " << trueAnswers[i].real() << " Ratio: " << ratioReal );
		BOOST_CHECK_MESSAGE(ratioImag < .20 || imagDiff < 5000 / 32768.0,
		"I: " << i << " Output: " << temp[i].imag() << " Answer: " << trueAnswers[i].imag() << " Ratio: " << ratioImag );

	}
}


CSIM_TEST_CASE(FFT_TWO_INPUTS)
{
    string infile("./data/fft/input/data_file_complex2.csv");
    string infile2("./data/fft/input/data_file_complex3.csv");
    string answersfile("./data/fft/answers/answers2.csv");
    string answersfile2("./data/fft/answers/answers3.csv");

    int i = 0;
    int realInput[32769] = {0}; // default values
    int imagInput[32769] = {0};
    ifstream in(infile.c_str());
    ifstream in2(infile2.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open " << infile);
    BOOST_REQUIRE_MESSAGE(in2.is_open(), "Could not open " << infile2);
    ifstream ans(answersfile.c_str());
    BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not open " << answersfile);
    ifstream ans2(answersfile2.c_str());
    BOOST_REQUIRE_MESSAGE(ans2.is_open(), "Could not open " << answersfile2);

    std::string token;
    string line;
    int inputs = 0;
    while(getline(in,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        realInput[inputs] = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        imagInput[inputs++] = intValue;

    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    int numAnswers = 0;
    std::vector<FixedComplex32> trueAnswers(32768); //array to hold answers from file
    while(getline(ans,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        trueAnswers[numAnswers].real(intValue / 32768.0);
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        trueAnswers[numAnswers++].imag(intValue / 32768.0);

    }//Reads in inputs from answers file. Parsing by commas. Format is: real,imag\n


    std::vector<FixedComplex32> output(32769); //Array to store answers
    int count = 0; //How many outputs have been collected

    int points = inputs;
    filter_io_t data;
    fixedfft fft(points ); //x point fft, y table size


	for (int j = 0; j < points; j++) {

		data = FixedComplex32(realInput[j] / 32768.0 ,imagInput[j] / 32768.0);
		fft.input(data);
		bool test = fft.output(data);
		if (test) {
		   output[count++] = data.fcn32;
		}//If output is ready
	}//Insert first set of data

    inputs = 0;


    while(getline(in2,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        realInput[inputs] = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        imagInput[inputs++] = intValue;
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    for (int j = 0; j < points; j++) {

        data = FixedComplex32(realInput[j] / 32768.0, imagInput[j] / 32768.0);
        fft.input(data);
        bool test = fft.output(data);
        if (test) {
        	output[count++] = data.fcn32;
        }
    }//second set of data, gets first set of answers out

    std::vector<FixedComplex32> temp(32769);
    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = output[i];
    }//Reformats data in correct order


    for (i = 0; i < inputs; i++) {
        double realRatio = abs((temp[i].real() - trueAnswers[i].real())/trueAnswers[i].real());
        double imagRatio = abs((trueAnswers[i].imag() - temp[i].imag() )/trueAnswers[i].imag());
        double realDiff  = abs(temp[i].real() - trueAnswers[i].real());
        double imagDiff  = abs(temp[i].imag() - trueAnswers[i].imag());
        BOOST_CHECK_MESSAGE(realRatio < .03 || realDiff < 1 / 32768.0 ,
                "I: " << i << " Output: " << temp[i].real() << " Answer: " << trueAnswers[i].real() << " Ratio: " << realRatio );
        BOOST_CHECK_MESSAGE(imagRatio < .03 || imagDiff < 1 / 32768.0,
                "I: " << i << " Output: " << temp[i].imag() << " Answer: " << trueAnswers[i].imag() << " Ratio: " << imagRatio );

        }//Ensures output and answers are reasonably close for first set of data

    numAnswers = 0;

    count = 0; //How many outputs have been collected
    for (int j = 0; j < points; j++) {

		data = FixedComplex32(0,0);
		fft.input(data);
		bool test = fft.output(data);
		if (test) {
			output[count++] = data.fcn32;
		}// if output is ready
	}//Get 2nd set of data out


    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = output[i];
    }//Reformats data in correct order

    while(getline(ans2,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        trueAnswers[numAnswers].real(intValue / 32768.0);
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        trueAnswers[numAnswers++].imag(intValue / 32768.0);

    }//Reads in answers from file. Parsing by commas. Format is: real,imag\n


    for (i = 0; i < inputs; i++) {
        double realRatio = abs((temp[i].real() - trueAnswers[i].real())/trueAnswers[i].real());
        double imagRatio = abs((trueAnswers[i].imag() - temp[i].imag() )/trueAnswers[i].imag());
        double realDiff = abs(temp[i].real() - trueAnswers[i].real());
        double imagDiff = abs(temp[i].imag() - trueAnswers[i].imag());
        BOOST_CHECK_MESSAGE(realRatio < .01 || realDiff < 4 / 32768.0,
           "I: " << i << " Output: " << temp[i].real() << " Answer: " << trueAnswers[i].real() << " Ratio: " << realDiff );
        BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 4 / 32768.0,
           "I: " << i << " Output: " << temp[i].imag() << " Answer: " << trueAnswers[i].imag() << " Ratio: " << imagDiff );

    }
}//Checks for two consecutive sets of inputs in the same FFT.

//
//
//CSIM_TEST_CASE(FFT_VERILOG)
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

CSIM_TEST_SUITE_END()

