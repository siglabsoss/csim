#include <test/unit_test.hpp>
#include <filters/fixed_fir.hpp>
#include <boost/tokenizer.hpp> //For parsing data from file
#include <utils/utils.hpp>
using namespace std;

CSIM_TEST_SUITE_BEGIN(FIRFilter)


void checkError(vector<FixedComplex16> outputs, vector<FixedComplex16> answers, float percent, float difference);

CSIM_TEST_CASE(REAL_FILTER)
{
    vector<FixedComplex16> input; //Vector to hold inputs
    vector<FixedComplex16> output; //Vector to hold outputs
    vector<FixedComplex16> answers; //Vector to hold answers
    vector<FixedComplex16> tap; //Vector to hold taps

    string data("./data/fir/input/data1_in.csv"); //Input data file
    input = readComplexFromCSV<FixedComplex16>(data); //Reads input file
    BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data);

    string taps("./data/fir/input/taps1.txt");
    tap = readComplexFromCSV<FixedComplex16>(taps);//Reads in taps from file
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps);

    string answersFile("./data/fir/answers/answers1.csv"); //Answers data file
    answers = readComplexFromCSV<FixedComplex16>(answersFile); //Reads answer file
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

    checkError(output,answers, .001, 0);
}//Test using only read numbers

CSIM_TEST_CASE(COMPLEX_FILTER)
{
	vector<FixedComplex16> input; //Vector to hold inputs
	vector<FixedComplex16> output; //Vector to hold outputs
	vector<FixedComplex16> answers;
	vector<FixedComplex16> tap;

	string data("./data/fir/input/data2_in.csv"); //Input data file
	input = readComplexFromCSV<FixedComplex16>(data); //Reads input file
	BOOST_REQUIRE_MESSAGE(!input.empty(), "Could not read from " << data);

    string taps("./data/fir/input/taps2.txt");
    tap = readComplexFromCSV<FixedComplex16>(taps); //Reads taps file
    BOOST_REQUIRE_MESSAGE(!tap.empty(), "Could not read from " << taps);

    string data3("./data/fir/answers/answers2.csv"); //Answers data file
    answers = readComplexFromCSV<FixedComplex16>(data3); //Reads answers file
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
    checkError(output,answers, .001, .001);

}//Test using complex numbers


void checkError(vector<FixedComplex16> outputs, vector<FixedComplex16> answers, float percent, float difference)
{
	for (unsigned int i = 0; i < answers.size(); i++) {
		    double ratioReal = abs((outputs[i].real() - answers[i].real())/answers[i].real());
		    double ratioImag = abs((answers[i].imag() - outputs[i].imag() )/answers[i].imag());
		    double realDiff = abs(outputs[i].real() - answers[i].real());
		    double imagDiff = abs(outputs[i].imag() - answers[i].imag());
			BOOST_CHECK_MESSAGE(ratioReal < percent || realDiff < difference  || realDiff == 0,
			"I: " << i << " Output: " << outputs[i].real() << " Answer: " << answers[i].real() << " Ratio: " << ratioReal );
			BOOST_CHECK_MESSAGE(ratioImag < percent || imagDiff < difference || imagDiff == 0,
			"I: " << i << " Output: " << outputs[i].imag() << " Answer: " << answers[i].imag() << " Ratio: " << ratioImag );
		}
}//Compares results of fft with answers. Takes in vector of outputs and answers, the max percent error as a float, and the max difference as an int

CSIM_TEST_SUITE_END()

