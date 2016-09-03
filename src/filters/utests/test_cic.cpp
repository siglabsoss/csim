#include <test/unit_test.hpp>
#include <vector> //For storing parsed data from file
#include <utils/utils.hpp>
#include <filters/fixed_cic.hpp>

using namespace std;

CSIM_TEST_SUITE_BEGIN(CICFilter)

void checkError(vector<FixedComplex16> outputs, vector<FixedComplex16> answers, float percent, int difference);

CSIM_TEST_CASE(REAL_FILTER) //Same as imaginary because there are only adds and subtracts
{

    vector<FixedComplex16> input; //Array to hold inputs
    vector<FixedComplex16> output; //Array to hold outputs
    vector<FixedComplex16> answers;

    string inFile("./data/cic/input/cic_data_in.txt"); //Input data file
    input = complexRead16Scaled(inFile);
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << inFile); //Reads input file

    string answersFile("./data/cic/answers/answers1.txt"); //Answers data file
    answers = complexRead16Scaled(answersFile);
	BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << answersFile); //Reads input file

    fixedcic cic(2, 2, 2); // decimate by 2, 2 registers before decimation, 2 registers after decimation
    for (unsigned int k = 0; k < input.size(); k++)
	{
    	filter_io_t data;
		data = input[k];
		cic.input(data); //Filters data
		bool test = cic.output(data);
		if (test) {
			output.push_back(data.fc);
		}
	}

    assert(output.size() == answers.size());

    checkError(output, answers, -10, -10);
}


void checkError(vector<FixedComplex16> outputs, vector<FixedComplex16> answers, float percent, int difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
		    double ratioReal = abs((outputs[i].real() - answers[i].real())/answers[i].real());
		    double ratioImag = abs((answers[i].imag() - outputs[i].imag() )/answers[i].imag());
		    double realDiff = abs(outputs[i].real() - answers[i].real());
		    double imagDiff = abs(outputs[i].imag() - answers[i].imag());
			BOOST_CHECK_MESSAGE(ratioReal < percent || realDiff < difference / 32768.0 || realDiff == 0,
			"I: " << i << " Output: " << outputs[i].real() << " Answer: " << answers[i].real() << " Ratio: " << ratioReal );
			BOOST_CHECK_MESSAGE(ratioImag < percent || imagDiff < difference / 32768.0 || imagDiff == 0,
			"I: " << i << " Output: " << outputs[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << ratioImag );
		}
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int


CSIM_TEST_SUITE_END()
