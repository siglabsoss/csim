#include <test/unit_test.hpp>

#include <cstring>
#include <stdio.h>//abs()
#include <iostream>// cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>
#include <utils/utils.hpp>
#include <filters/xcorrelator.hpp>

using namespace std;
using namespace boost;

CSIM_TEST_SUITE_BEGIN(xcorrelatorFilter)

void checkError(vector<FixedComplex32> outputs, vector<FixedComplex32> answers, float percent, int difference);

CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_64)
{
    string inFile("data/xcorrelator/input/data_file_complex1.csv");
    string answersFile("data/xcorrelator/answers/answers1.csv");

    vector<FixedComplex32> input = complexRead32Unscaled(inFile);
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inFile);

    vector<FixedComplex32 > output;
    XCorrelator x(input.size());
    output = x.xCorrelate(input,input);

    vector<FixedComplex32> answers = complexRead32Unscaled(answersFile);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << inFile);

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

    checkError(output, answers, .005, 1 );
}


CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_128)
{
    string inFile("data/xcorrelator/input/data_file_complex2.csv");
    string answersFile("data/xcorrelator/answers/answers2.csv");

    vector<FixedComplex32> input = complexRead32Unscaled(inFile);
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inFile);

    vector<FixedComplex32 > output;
    XCorrelator x(input.size());
    output = x.xCorrelate(input,input);

    vector<FixedComplex32> answers = complexRead32Unscaled(answersFile);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << inFile);

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

    checkError(output, answers, .04, 1 );
}


CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_256)
{
	string inFile("data/xcorrelator/input/data_file_complex3.csv");
	string answersFile("data/xcorrelator/answers/answers3.csv");

	vector<FixedComplex32> input = complexRead32Unscaled(inFile);
	BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inFile);

	vector<FixedComplex32 > output;
	XCorrelator x(input.size());
	output = x.xCorrelate(input,input);

	vector<FixedComplex32> answers = complexRead32Unscaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << inFile);

	BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

	checkError(output, answers, .015, 1 );
}

CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_512)
{
	string inFile("data/xcorrelator/input/data_file_complex4.csv");
	string answersFile("data/xcorrelator/answers/answers4.csv");

	vector<FixedComplex32> input = complexRead32Unscaled(inFile);
	BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inFile);

	vector<FixedComplex32 > output;
	XCorrelator x(input.size());
	output = x.xCorrelate(input,input);

	vector<FixedComplex32> answers = complexRead32Unscaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << inFile);

	BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

	checkError(output, answers, .05, 1 );
}

CSIM_TEST_CASE(COMPLEX_CROSS_CORRELATOR_OCTAVE_COMPARISON_32)
{

	string inFile("data/xcorrelator/input/data_file_complex5.csv");
	string inFile2("data/xcorrelator/input/data_file_complex6.csv");
	string answersFile("data/xcorrelator/answers/answers5.csv");

	vector<FixedComplex32> x = complexRead32Unscaled(inFile);
	BOOST_REQUIRE_MESSAGE(!x.empty(), "Could not read from " << inFile);


	vector<FixedComplex32> y = complexRead32Unscaled(inFile2);
	BOOST_REQUIRE_MESSAGE(!y.empty(), "Could not read from " << inFile2);

	vector<FixedComplex32 > output;
	XCorrelator xc(x.size());
	output = xc.xCorrelate(x, y);

	vector<FixedComplex32> answers = complexRead32Unscaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << inFile);

	BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

	checkError(output, answers, .006, 0 );
}


void checkError(vector<FixedComplex32> outputs, vector<FixedComplex32> answers, float percent, int difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
		double ratioReal = abs((outputs[i].real() - answers[i].real())/answers[i].real());
		double ratioImag = abs((outputs[i].imag() - answers[i].imag() )/answers[i].imag());
		double realDiff = abs(outputs[i].real() - answers[i].real());
		double imagDiff = abs(outputs[i].imag() - answers[i].imag());
		BOOST_CHECK_MESSAGE(ratioReal < percent || realDiff < difference / 32768.0 ,
		"I: " << i << " Output: " << outputs[i].real() << " Answer: " << answers[i].real() << " Ratio: " << ratioReal );
		BOOST_CHECK_MESSAGE(ratioImag < percent || imagDiff < difference / 32768.0,
		"I: " << i << " Output: " << outputs[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << ratioImag );
	}
}//Compares results of filter with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int


CSIM_TEST_SUITE_END()
