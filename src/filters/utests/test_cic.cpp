#include <test/unit_test.hpp>
#include <iostream>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <utils/utils.hpp>
#include <filters/fixed_cic.hpp>

using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(CICFilter)

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

    fixedcic cic(2, 2, 2);
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
    for (unsigned int k = 0; k < answers.size(); k++) {
        BOOST_CHECK_MESSAGE(abs(output[k].real() - answers[k].real()) < 1,
                output[k].real() << " is not the same as " << answers[k].real() << " ");
    } //Compares all outputs with solution to ensure they are .001 within each other.
}

CSIM_TEST_SUITE_END()
