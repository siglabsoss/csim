#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Complex

#include <boost/test/included/unit_test.hpp>//boost main()
#include <cstring>
#include <stdio.h>//abs()
#include <iostream>// cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include <filters/stitcher.hpp>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE(PYTHON_COMPARISON)
{
    int waves[2] = { 1, 0 };
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
    outdatas = stitch.stitch(400, 1000, 100, datas); //(int numsamples, int sampleRate, int frequency) stitches data with clockup and clockdown waves

    int size = outdatas.size();
    string outFile = "data/stitcher/output/stitching2.txt"; //Output data file
    ofstream out(outFile.c_str());
    if (!out.is_open()) {
           cout << "error reading" << endl;
           BOOST_REQUIRE_MESSAGE(0 == 1, "Could not write to data/stitcher/output/stitching2.txt");
    }

    for (int i = 0; i < size; i++) {
        out << outdatas[i].real / 32768.0 << "," << outdatas[i].imag / 32768.0
                << endl;
        //	cout << outdatas[i];
    }
    cout << "DONE" << endl;

    char ** ptr;
    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    vector<double> answers; //Vector to hold answer data read from file
    string line;
    string data2("data/stitcher/answers/stitcher_answers.txt"); //Answers data file
    ifstream in2(data2.c_str());
    if (!in2.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from data/stitcher/answers/stitcher_answers.txt");
    }

    int l = 0;
    while (getline(in2, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        answers.push_back(atof(vec[0].c_str()));
        l++;
    } //Gets each line of data. Allows separation of real and imaginary in future. Currently only real.

    for (int i = 0; i < 200; i++) {
        BOOST_CHECK_MESSAGE(abs(outdatas[i].real / 32768.0 - answers[i]) < .005,
                i << ": " << outdatas[i].real/32768.0 << " is not equal to " << answers[i]);
    } //Checks output data and answers to verify they are within .005 of each other

} //Compares python data with stitcher output data.

BOOST_AUTO_TEST_CASE(CONCATENATION)
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
    } //Ensures 2 waves of the same tpye concatenated is the same as one long wave of the same type
    cout << "DONE" << endl;

} //Ensures 2 waves of the same tpye concatenated is the same as one long wave of the same type
