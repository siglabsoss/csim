#include <test/unit_test.hpp>
#include <filters/fixed_iir.hpp>
#include <utils/utils.hpp>

using namespace std;

CSIM_TEST_SUITE_BEGIN(IIRFilter)

void checkError(vector<FixedComplex16> outputs, vector<FixedComplex16> answers, float percent, float difference);

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
    input = readComplexFromCSV<FixedComplex16>(inFile);

    string ataps("./data/iir/input/ataps.txt");
    atap = readComplexFromCSV<FixedComplex16>(ataps);

    string btaps("./data/iir/input/btaps.txt");
    btap = readComplexFromCSV<FixedComplex16>(btaps);

    string answersFile("./data/iir/answers/answers1.txt"); //Answers data file
    answers = readComplexFromCSV<FixedComplex16>(answersFile);


    fixediir iir(atap, btap);
    filter_io_t data;
    for (unsigned int j = 0; j < input.size(); j ++) {
        data = input[j];
        iir.input(data); //Filters data
        output.push_back(iir.m_output);
    }

    assert(output.size() == answers.size());//Length of inputs is length of answers/outputs.

    checkError(output, answers, .001, .001);

}


void checkError(vector<FixedComplex16> outputs, vector<FixedComplex16> answers, float percent, float difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
		    double ratioReal = abs((outputs[i].real() - answers[i].real())/answers[i].real());
		    double ratioImag = abs((answers[i].imag() - outputs[i].imag() )/answers[i].imag());
		    double realDiff = abs(outputs[i].real() - answers[i].real());
		    double imagDiff = abs(outputs[i].imag() - answers[i].imag());
			BOOST_CHECK_MESSAGE(ratioReal < percent || realDiff < difference || realDiff == 0,
			"I: " << i << " Output: " << outputs[i].real() << " Answer: " << answers[i].real() << " Ratio: " << ratioReal );
			BOOST_CHECK_MESSAGE(ratioImag < percent || imagDiff < difference || imagDiff == 0,
			"I: " << i << " Output: " << outputs[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << ratioImag );
		}
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

CSIM_TEST_SUITE_END()
