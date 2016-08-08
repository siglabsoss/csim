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

CSIM_TEST_CASE(FFT_OCTAVE)
{
	string infile("./data/fft/input/data_file_complex1.csv");
	string answersfile("./data/fft/answers/answers1.csv");

	int i = 0;
	int realInput[32769] = {0}; // default values
	int imagInput[32769] = {0};
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
	FixedComplex<32> trueAnswers[32768];
	while(getline(ans,line)) {

		istringstream ss(line);
		getline(ss, token, ',');
		stringstream strValue;
		strValue << token;
		int intValue;
		strValue >> intValue;
		trueAnswers[numAnswers].real = intValue;
		getline(ss, token, ',');
		stringstream strValue2;
		strValue2 << token;
		strValue2 >> intValue;
		trueAnswers[numAnswers++].imag = intValue;

	}//Reads in inputs from file. Parsing by commas. Format is: real,imag\n


	FixedComplex<32> answers[32769];//Array to store answers
	int count = 0; //How many outputs have been collected

	int points = inputs;
	filter_io_t data;
	data.type =  IO_TYPE_FIXED_COMPLEX_32;
	fixedfft fft(points ); //x point fft, y table size

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {

			data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				answers[count++] = data.fc32;
			}
		}
	}


	assert(count == inputs);
	// If you want bits to be reversed

	FixedComplex<32> temp[32769];
	for (i = 0; i < inputs; i++) {
		temp[reverseBits(inputs, i)] = answers[i];
	}//Reformats data in correct order


	for (i = 0; i < inputs; i++) {
		BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .05 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 1 ,
		"I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << "Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
		BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .05 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 1,
		"I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << "Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );

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
    FixedComplex<32> trueAnswers[32768];
    while(getline(ans,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        trueAnswers[numAnswers].real = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        trueAnswers[numAnswers++].imag = intValue;

    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n


    FixedComplex<32> answers[32769];//Array to store answers
    int count = 0; //How many outputs have been collected

    int points = inputs;
    filter_io_t data;
    data.type =  IO_TYPE_FIXED_COMPLEX_32;
    fixedfft fft(points ); //x point fft, y table size

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < points; j++) {

            data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
            fft.input(data);
            bool test = fft.output(data);
            if (test) {
               answers[count++] = data.fc32;
            }
        }
    }
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

        data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
        fft.input(data);
        bool test = fft.output(data);
        if (test) {
           answers[count++] = data.fc32;
        }
    }

    FixedComplex<32> temp[32769];
    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = answers[i];
    }//Reformats data in correct order


    for (i = 0; i < inputs; i++) {
        BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .02 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 1 ,
                "I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << " Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
        BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .02 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 1,
                "I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << " Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );

        }

    numAnswers = 0;

    while(getline(ans2,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        trueAnswers[numAnswers].real = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        trueAnswers[numAnswers++].imag = intValue;

    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    count = 0; //How many outputs have been collected
    data.type =  IO_TYPE_FIXED_COMPLEX_32;


    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {

            data.fc32 = FixedComplex<32>(0,0);
            fft.input(data);
            bool test = fft.output(data);
            if (test) {
                answers[count++] = data.fc32;
            }
        }
    }



    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = answers[i];
    }//Reformats data in correct order


    for (i = 0; i < inputs; i++) {
        BOOST_CHECK_MESSAGE(abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) < .01 || abs(abs(temp[i].real) - abs(trueAnswers[i].real)) < 4 ,
           "I: " << i << " Output: " << temp[i].real << " Answer: " << trueAnswers[i].real << " Ratio: " << abs((temp[i].real - trueAnswers[i].real)/(float)trueAnswers[i].real) );
        BOOST_CHECK_MESSAGE(abs((trueAnswers[i].imag - temp[i].imag )/(float)trueAnswers[i].imag) < .01 || abs(abs(temp[i].imag) - abs(trueAnswers[i].imag)) < 4,
           "I: " << i << " Output: " << temp[i].imag << " Answer: " << trueAnswers[i].imag << " Ratio: " << abs((temp[i].imag - trueAnswers[i].imag)/(float)trueAnswers[i].imag) );

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
//       FixedComplex<32> bitReversedInput[32768];
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
//       FixedComplex<32> trueAnswers[32768];
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
////        FixedComplex<32> temp[32769];
////        for (i = 0; i < inputs; i++) {
////            temp[i] = bitReversedOutput[reverseBits(inputs, i)];
////        }//Reformats data in correct order
//
//
////       FixedComplex<32> temp[32769];
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

