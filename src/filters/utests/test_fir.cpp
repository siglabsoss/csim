#include <test/unit_test.hpp>
#include <filters/fixed_fir.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <utils/utils.hpp>
using namespace std;

CSIM_TEST_SUITE_BEGIN(FIRFilter)

CSIM_TEST_CASE(REAL_FILTER)
{
    vector<FixedComplex16> input; //Vector to hold inputs
    vector<FixedComplex16> output; //Vector to hold outputs
    vector<FixedComplex16> answers; //Vector to hold answers
    vector<FixedComplex16> tap; //Vector to hold taps

    string data("./data/fir/input/data1_in.csv"); //Input data file
    input = complexRead16Unscaled(data); //Reads input file
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data);

    string taps("./data/fir/input/taps1.txt");
    tap = complexRead16Unscaled(taps);//Reads in taps from file
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps);

    string answersFile("./data/fir/answers/answers1.csv"); //Answers data file
    answers = complexRead16Unscaled(answersFile); //Reads answer file
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << answersFile);

    FixedFIR fir(tap); //Creates instance of fixed FIR filter given a vector of taps.
    for (unsigned int k = 0; k < input.size(); k++)
    {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fc);
    }//Filters using FIR

    for (unsigned int k = 0; k < answers.size(); k++) {
        BOOST_CHECK_MESSAGE(
			abs(output[k].real() - answers[k].real())
					< .001,
			output[k].real() << " is not the same as " << answers[k].real());
    }//Compares all outputs with solution to ensure they are .001 within each other.
}//Test using only read numbers

CSIM_TEST_CASE(COMPLEX_FILTER)
{
	vector<FixedComplex16> input; //Vector to hold inputs
	vector<FixedComplex16> output; //Vector to hold outputs
	vector<FixedComplex16> answers;
	vector<FixedComplex16> tap;

	string data("./data/fir/input/data2_in.csv"); //Input data file
	input = complexRead16Unscaled(data); //Reads input file
	BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data);

    string taps("./data/fir/input/taps2.txt");
    tap = complexRead16Unscaled(taps); //Reads taps file
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps);

    string data3("./data/fir/answers/answers2.csv"); //Answers data file
    answers = complexRead16Unscaled(data3); //Reads answers file
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << data3);

    FixedFIR fir(tap); //Creates instance of fixed FIR filter given vector of taps.
    for (unsigned int k = 0; k < input.size(); k++) {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fc);
     }//Filters data

    assert (output.size() == answers.size());//Checks number of outputs

    for (unsigned int k = 0; k < answers.size(); k++) {
        BOOST_CHECK_MESSAGE(
			abs(output[k].real() - answers[k].real()) < .001,
			input[k].real() << " is not the same as " << answers[k]);
        BOOST_CHECK_MESSAGE(
			abs(output[k].imag() - answers[k].imag()) < .001,
			output[k].imag() << " is not the same as " << answers[k]);
    } //Compares all outputs with solution to ensure they are .001 within each other.
}//Test using complex numbers

CSIM_TEST_SUITE_END()

