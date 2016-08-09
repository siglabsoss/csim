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


CSIM_TEST_SUITE_BEGIN(FFT_AND_IFFT)

CSIM_TEST_CASE(FFT_IFFT)
{
	cout << "STARTING FFT AND IFFT" << endl;
	string infile("./data/fft/input/data_file_complex1.csv");

	int i = 0;
	ifstream in(infile.c_str());
	BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not open " << infile);
	FixedComplex<32> originalInput[32768];
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
	originalInput[inputs].real = intValue;
	getline(ss, token, ',');
	stringstream strValue2;
	strValue2 << token;
	strValue2 >> intValue;
	originalInput[inputs++].imag = intValue;
	}//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

	FixedComplex<32> answers[32769];//Array to store answers
	int count = 0; //How many outputs have been collected

	int points = inputs;
	filter_io_t data;
	data.type =  IO_TYPE_FIXED_COMPLEX_32;
	fixedfft fft(points);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data.fc32 = originalInput[j];
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				answers[count++] = data.fc32;
			}

		}
	}
//
//	FixedComplex<32> temp[32769];
//	for (i = 0; i < inputs; i++) {
//		temp[reverseBits(inputs, i)] = answers[i];
//	}//Reformats data in correct order
//
//	count = 0; //How many outputs have been collected
//
//	points = inputs;
//
//	data.type =  IO_TYPE_FIXED_COMPLEX_32;
//	fixedifft ifft(points);
//
//	for (int i = 0; i < 2; i++) {
//		for (int j = 0; j < points; j++) {
//
//			data.fc32 = temp[j];
//			ifft.input(data);
//			bool test = ifft.output(data);
//			if (test) {
//				answers[count++] = data.fc32;
//			}
//		}
//	}
//
//
//	for (i = 0; i < inputs; i++) {
//		temp[reverseBits(inputs, i)] = answers[i];
//	}//Reformats data in correct order
//
//
//	for (i = 0; i < inputs; i++) {
//		BOOST_CHECK_MESSAGE(abs((temp[i].real - originalInput[i].real)/(float)originalInput[i].real) <  .01 || abs(abs(temp[i].real) - abs(originalInput[i].real)) < 5 ,
//		"I: " << i << " Output: " << temp[i].real << " Answer: " << originalInput[i].real << " Ratio: " << abs((temp[i].real - originalInput[i].real)/(float)originalInput[i].real) );
//		BOOST_CHECK_MESSAGE(abs((originalInput[i].imag - temp[i].imag )/(float)originalInput[i].imag) < .01 || abs(abs(temp[i].imag) - abs(originalInput[i].imag)) < 5,
//		"I: " << i << " Output: " << temp[i].imag << " Answer: " << originalInput[i].imag << " Ratio: " << abs((temp[i].imag - originalInput[i].imag)/(float)originalInput[i].imag) );
//	}
	cout <<"DONE WITH FFT AND IFFT" << endl;
}//Used for Running FFT and then IFFT

CSIM_TEST_SUITE_END()
