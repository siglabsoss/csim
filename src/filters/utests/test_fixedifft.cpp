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

using namespace std;


CSIM_TEST_SUITE_BEGIN(FixedIFFT)

CSIM_TEST_CASE(IFFT_OCTAVE)
{
	string infile("./data/ifft/input/data_file_complex1.csv");
	string answersfile("./data/ifft/answers/answers1.csv");

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

	}//Reads in inputs from file. Parsing by commas. Format is: real,imag\n


	std::vector<FixedComplex32> answers(32769);
	int count = 0; //How many outputs have been collected

	int points = inputs;
	filter_io_t data;
	fixedifft ifft(points); //8 point fft

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = FixedComplex32(realInput[j] / 32768.0,imagInput[j] / 32768.0);
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				answers[count++] = data.fcn32;
			}
		}
	}

	assert(count == inputs);
	// If you want bits to be reversed

	std::vector<FixedComplex32> temp(32769);
	for (i = 0; i < inputs; i++) {
		temp[reverseBits(inputs, i)] = answers[i];
	}//Reformats data in correct order


	for (i = 0; i < inputs; i++) {
	    double realRatio = abs((temp[i].real() - trueAnswers[i].real())/trueAnswers[i].real());
	    double imagRatio = abs((trueAnswers[i].imag() - temp[i].imag() )/trueAnswers[i].imag());
	    double realDiff = abs(temp[i].real() - trueAnswers[i].real());
	    double imagDiff = abs(temp[i].imag() - trueAnswers[i].imag());
		BOOST_CHECK_MESSAGE(realRatio < .01 || realDiff  < 3 / 32768.0,
		"I: " << i << " Output: " << temp[i].real() << " Answer: " << trueAnswers[i].real() << " Ratio: " << realRatio );
		BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 3 / 32768.0,
		"I: " << i << " Output: " << temp[i].imag() << " Answer: " << trueAnswers[i].imag() << " Ratio: " << realRatio );
	}
}


CSIM_TEST_CASE(IFFT_TWO_INPUTS)
{
    string infile("../data/ifft/input/data_file_complex2.csv");
    string infile2("../data/ifft/input/data_file_complex3.csv");
    string answersfile("../data/ifft/answers/answers2.csv");
    string answersfile2("../data/ifft/answers/answers3.csv");

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

    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n


    std::vector<FixedComplex32> answers(32769);
    int count = 0; //How many outputs have been collected

    int points = inputs;
    filter_io_t data;
    fixedifft ifft(points); //8 point fft

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < points; j++) {

            data = FixedComplex32(realInput[j] / 32768.0,imagInput[j] / 32768.0);
            ifft.input(data);
            bool test = ifft.output(data);
            if (test) {
               answers[count++] = data.fcn32;
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

        data = FixedComplex32(realInput[j] / 32768.0, imagInput[j] / 32768.0);
        ifft.input(data);
        bool test = ifft.output(data);
        if (test) {
           answers[count++] = data.fcn32;
        }
    }

    std::vector<FixedComplex32> temp(32769);
    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = answers[i];
    }//Reformats data in correct order


    for (i = 0; i < inputs; i++) {
        double realRatio = abs((temp[i].real() - trueAnswers[i].real())/trueAnswers[i].real());
        double imagRatio = abs((trueAnswers[i].imag() - temp[i].imag() )/trueAnswers[i].imag());
        double realDiff = abs(temp[i].real() - trueAnswers[i].real());
        double imagDiff = abs(temp[i].imag() - trueAnswers[i].imag());
        BOOST_CHECK_MESSAGE(realRatio < .01 || realDiff < 2 / 32768.0 ,
                "I: " << i << " Output: " << temp[i].real() << " Answer: " << trueAnswers[i].real() << " Ratio: " << realRatio );
        BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 2 / 32768.0,
                "I: " << i << " Output: " << temp[i].imag() << " Answer: " << trueAnswers[i].imag() << " Ratio: " << imagRatio );

        }

    numAnswers = 0;

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

    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    count = 0; //How many outputs have been collected

	for (int j = 0; j < points; j++) {

		data = FixedComplex32(0,0);
		ifft.input(data);
		bool test = ifft.output(data);
		if (test) {
			answers[count++] = data.fcn32;
		}
	}


    for (i = 0; i < inputs; i++) {
        temp[reverseBits(inputs, i)] = answers[i];
    }//Reformats data in correct order

    for (i = 0; i < inputs; i++) {
        double realRatio = abs((temp[i].real() - trueAnswers[i].real())/trueAnswers[i].real());
        double imagRatio = abs((trueAnswers[i].imag() - temp[i].imag() )/trueAnswers[i].imag());
        double realDiff = abs(temp[i].real() - trueAnswers[i].real());
        double imagDiff = abs(temp[i].imag() - trueAnswers[i].imag());
        BOOST_CHECK_MESSAGE(realRatio < .01 || realDiff < 2 / 32768.0,
           "I: " << i << " Output: " << temp[i].real() << " Answer: " << trueAnswers[i].real() << " Ratio: " << realRatio );
        BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 2,
           "I: " << i << " Output: " << temp[i].imag() << " Answer: " << trueAnswers[i].imag() << " Ratio: " << imagRatio );
    }
}//Checks for two consecutive sets of inputs in the same IFFT.


CSIM_TEST_CASE(FFT_IFFT)
{
	string infile("../data/fft/input/data_file_complex1.csv");

	int i = 0;
	ifstream in(infile.c_str());
	BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open " << infile);
	std::vector<FixedComplex32> originalInput(32768);
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
	originalInput[inputs].real(intValue / 32768.0);
	getline(ss, token, ',');
	stringstream strValue2;
	strValue2 << token;
	strValue2 >> intValue;
	originalInput[inputs++].imag(intValue / 32768.0);
	}//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

	std::vector<FixedComplex32> answers(32769);
	int count = 0; //How many outputs have been collected

	int points = inputs;
	filter_io_t data;
	fixedfft fft(points);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = originalInput[j];
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				answers[count++] = data.fcn32;
			}
		}
	}

    std::vector<FixedComplex32> temp(32769);
	for (i = 0; i < inputs; i++) {
		temp[reverseBits(inputs, i)] = answers[i];
	}//Reformats data in correct order

	count = 0; //How many outputs have been collected

	points = inputs;

	fixedifft ifft(points);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {

			data = temp[j];
			ifft.input(data);
			bool test = ifft.output(data);
			if (test) {
				answers[count++] = data.fcn32;
			}
		}
	}


	for (i = 0; i < inputs; i++) {
		temp[reverseBits(inputs, i)] = answers[i];
	}//Reformats data in correct order


	for (i = 0; i < inputs; i++) {
        double realRatio = abs((temp[i].real() - originalInput[i].real())/originalInput[i].real());
        double imagRatio = abs((originalInput[i].imag() - temp[i].imag() )/originalInput[i].imag());
        double realDiff = abs(temp[i].real() - originalInput[i].real());
        double imagDiff = abs(temp[i].imag() - originalInput[i].imag());
		BOOST_CHECK_MESSAGE(realRatio <  .01 || realDiff < 5 / 32768.0,
		"I: " << i << " Output: " << temp[i].real() << " Answer: " << originalInput[i].real() << " Ratio: " << realRatio );
		BOOST_CHECK_MESSAGE(imagRatio < .01 || imagDiff < 5 / 32768.0,
		"I: " << i << " Output: " << temp[i].imag() << " Answer: " << originalInput[i].imag() << " Ratio: " << abs((temp[i].imag() - originalInput[i].imag())/(float)originalInput[i].imag()) );
	}
}//Used for Running FFT and then IFFT


CSIM_TEST_SUITE_END()

