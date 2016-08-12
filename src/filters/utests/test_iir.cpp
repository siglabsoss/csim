#include <test/unit_test.hpp>
#include <iostream>
#include <filters/fixed_iir.hpp>
#include <utils/utils.hpp>

using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(IIRFilter)

CSIM_TEST_CASE(REAL_FILTER)
{
	int NUM_X_REGISTERS = 2;
	int  NUM_Y_REGISTERS = 5;
	vector<FixedComplex16 > a (5);
	vector<FixedComplex16> b(2); //b coefficients
	float temp1[5] = { 1, .5, .5, .6, .7 }; // a coefficients
	float temp2[2] = { .5, .5 }; //b coefficients

    for (int i = 0; i < NUM_Y_REGISTERS; i++) {
        a[i].real(temp1[i]);
    }

    for (int i = 0; i < NUM_X_REGISTERS; i++) {
        b[i].real(temp2[i]);
    }

    FixedComplex16 output[1024];
    FixedComplex16 input[1024];

    for (int i = 1; i < 5; i++) {
        input[i - 1].real((double) i / 10.00);
    }

    fixediir iir(a, b);

    filter_io_t data;
    for (int i = 0; i < 4; i ++) {
      data = input[i];
      iir.input(data); //Filters data
      output[i] = iir.m_output;
    }
    BOOST_CHECK(abs(output[0].real() - .05) < .001);
    BOOST_CHECK(abs(output[1].real() - .125) < .001);
    BOOST_CHECK(abs(output[2].real() - .1625) < .001);
    BOOST_CHECK(abs(output[3].real() - .176250) < .001);
}


CSIM_TEST_CASE(COMPLEX_FILTER)
{
    vector <FixedComplex16> input; //Vector to hold inputs
    vector<FixedComplex16> output; //Vector to hold outputs
    vector <FixedComplex16> answers; //Vector to hold inputs
    vector<FixedComplex16> atap; //Vector for A taps
    vector<FixedComplex16> btap; //Vector for B taps

    string inFile("./data/iir/input/data1_in.csv"); //Input data file
    input = complexRead16Unscaled(inFile);

    string ataps("./data/iir/input/ataps.txt");
    atap = complexRead16Unscaled(ataps);

    string btaps("./data/iir/input/btaps.txt");
    btap = complexRead16Unscaled(btaps);

    string answersFile("./data/iir/answers/answers1.txt"); //Answers data file
    answers = complexRead16Unscaled(answersFile);


    fixediir iir(atap, btap);
    filter_io_t data;
    for (int j = 0; j < input.size(); j ++) {
        data = input[j];
        iir.input(data); //Filters data
        output.push_back(iir.m_output);
    }

    assert(output.size() == answers.size());//Length of inputs is length of answers/outputs.

    for (int k = 0; k < output.size(); k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real() - answers[k].real()) < .001,
                output[k].real() << " is not the same as " << answers[k].real());
        BOOST_CHECK_MESSAGE(
                abs(output[k].imag() - answers[k].imag()) < .001,
                output[k].imag() << " is not the same as " << answers[k].imag());
    }
}

CSIM_TEST_SUITE_END()
