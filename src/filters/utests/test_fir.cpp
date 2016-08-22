#include <test/unit_test.hpp>
#include <filters/fixed_fir.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <utils/utils.hpp>
using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(FIRFilter)

CSIM_TEST_CASE(REAL_FILTER)
{
    vector<FixedComplex16> input; //Vector to hold inputs
    vector<FixedComplex16> output; //Vector to hold outputs
    vector<FixedComplex16> answers;
    vector<FixedComplex16> tap;

    string data("./data/fir/input/data1_in.csv"); //Input data file
    input = complexRead16Unscaled(data);
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data); //Reads input file

    string taps("./data/fir/input/taps1.txt");
    tap = complexRead16Unscaled(taps);//Reads in taps from file
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps); //Reads taps file

    string answersFile("./data/fir/answers/answers1.csv"); //Answers data file
    answers = complexRead16Unscaled(answersFile);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << answersFile); //Reads answer file

    FixedFIR fir(tap); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < input.size(); k++)
    {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fcn);
    }//Filters using FIR

    for (int k = 0; k < answers.size(); k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real() - answers[k].real())
                        < .001,
                output[k].real() << " is not the same as " << answers[k].real());

    }//Compares all outputs with solution to ensure they are .001 within each other.
}

CSIM_TEST_CASE(COMPLEX_FILTER)
{
	vector<FixedComplex16> input; //Vector to hold inputs
	vector<FixedComplex16> output; //Vector to hold outputs
	vector<FixedComplex16> answers;
	vector<FixedComplex16> tap;

	string data("./data/fir/input/data2_in.csv"); //Input data file
	input = complexRead16Unscaled(data);
	BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data); //Reads input file

    string taps("./data/fir/input/taps2.txt");
    tap = complexRead16Unscaled(taps);
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps); //Reads taps file

    string data3("./data/fir/answers/answers2.csv"); //Answers data file
    answers = complexRead16Unscaled(data3);
    BOOST_REQUIRE_MESSAGE(!answers.empty(), "Could not read from " << data3); //Reads answers file

    FixedFIR fir(tap); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < input.size(); k++) {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fcn);
     }//Filters data

    assert (output.size() == answers.size());//Checks number of outputs

    for (int k = 0; k < answers.size(); k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real() - answers[k].real()) < .001,
                input[k].real() << " is not the same as " << answers[k]);
        BOOST_CHECK_MESSAGE(
                abs(output[k].imag() - answers[k].imag()) < .001,
                output[k].imag() << " is not the same as " << answers[k]);
    } //Compares all outputs with solution to ensure they are .001 within each other.
}

CSIM_TEST_SUITE_END()

