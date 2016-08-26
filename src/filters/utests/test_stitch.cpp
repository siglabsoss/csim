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
#include <filters/stitcher.hpp>

using namespace std;
using namespace boost;

CSIM_TEST_SUITE_BEGIN(StitcherFilter)

void writeToFile(vector<FixedComplex<32> > outdatas)
{
	int size = outdatas.size();
	string outFile = "data/stitcher/output/stitching.txt"; //Output data file
	ofstream out(outFile.c_str());
	if (!out.is_open()) {
		   cout << "error reading" << endl;
		   BOOST_REQUIRE_MESSAGE(0 == 1, "Could not write to data/stitcher/output/stitching.txt");
	}

	for (int i = 0; i < size; i++) {
		out << outdatas[i].real / 32768.0 << "," << outdatas[i].imag / 32768.0 << endl;
	}

}

std::vector<double> readFromFile(std::string inFile)
{
	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;

	vector<string> vec;
	vector<double> answers; //Vector to hold answer data read from file
	string line;
	string data2 = inFile; //Answers data file
	ifstream in2(data2.c_str());

	BOOST_REQUIRE_MESSAGE(in2.is_open(), "Could not read from " << inFile);

	int l = 0;
	while (getline(in2, line)) {
	Tokenizer tok(line);
	vec.assign(tok.begin(), tok.end());
	answers.push_back(atof(vec[0].c_str()));
	l++;
	} //Gets each line of data. Allows separation of real and imaginary in future. Currently only real.
	return answers;
}

CSIM_TEST_CASE(PYTHON_COMPARISON)
{

    int waves[2] = { 1, 0 };
    int samples[2] = { 200, 200 };
    int num_sections = sizeof(samples) / sizeof(samples[0]);
    vector<FixedComplex<32> > datas;
    int m = 32768;
	for (int i = 0; i < 400; i++) {

		FixedComplex<32> j(m, m);
		datas.push_back(j);
		m*=-1;
	}

    Stitcher stitch(waves, samples, num_sections); //Creates stitcher
    vector<FixedComplex<32> > outdatas; //Vector for output data
    outdatas = stitch.stitch(400, 1000, 100, datas); //(int numsamples, int sampleRate, int frequency) stitches data with clockup and clockdown waves

    writeToFile(outdatas); //to view later

    vector<double> answers = readFromFile("data/stitcher/answers/answers1.txt");

    for (int i = 0; i < 400; i++) {
        BOOST_CHECK_MESSAGE(abs(outdatas[i].real / 32768.0 - answers[i]) < .005,
                i << ": " << outdatas[i].real/32768.0 << " is not equal to " << answers[i]);
    } //Checks output data and answers to verify they are within .005 of each other

} //Compares python data with stitcher output data.

CSIM_TEST_CASE(CONCATENATION)
{

    int waves[2] = { 1, 1 };
    int samples[2] = { 200, 200 };
    int num_sections = sizeof(samples) / sizeof(samples[0]);
    vector<FixedComplex<32> > datas;
    for (int k = 0; k < 7; k++)
        for (int i = 0; i < 32768; i++) {
            FixedComplex<32> j(i, i);
            datas.push_back(j);
        }

    Stitcher stitch(waves, samples, num_sections); //Creates stitcher
    vector<FixedComplex<32> > outdatas; //Vector for output data
    outdatas = stitch.stitch(400, 1000, 100, datas); //Two separate waves

    int waves2[1] = { 1 };
    int samples2[1] = { 400 };
    int num_sections2 = sizeof(samples) / sizeof(samples[0]);

    Stitcher stitch2(waves2, samples2, num_sections2); //Creates stitcher
    vector<FixedComplex<32> > outdatas2; //Vector for output data
    outdatas2 = stitch2.stitch(400, 1000, 100, datas); //One long wave

    int size = outdatas.size();

    for (int i = 0; i < size; i++) {
        BOOST_CHECK_MESSAGE(
                (outdatas[i].real / 32768.0 == outdatas2[i].real / 32768.0),
                i << ": " << outdatas[i].real/32768.0 << " is not equal to " << outdatas2[i].real/32768.0);
    } //Ensures 2 waves of the same type concatenated is the same as one long wave of the same type


} //Ensures 2 waves of the same type concatenated is the same as one long wave of the same type

CSIM_TEST_SUITE_END()
