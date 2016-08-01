#include <test/unit_test.hpp>

#include <cstring>
#include <stdio.h>//abs()
#include <iostream>// cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include <filters/xcorrelator.hpp>

using namespace std;
using namespace boost;

CSIM_TEST_SUITE_BEGIN(xcorrelatorFilter)

CSIM_TEST_CASE(OCTAVE_COMPARISON)
{

    string inFile("data/xcorrelator/input/data_file_complex1.csv");
    ifstream in(inFile.c_str());
    if (!in.is_open()) {
       cout << "error reading" << endl;
       BOOST_REQUIRE_MESSAGE(0 == 1, "Could not write to " << inFile);
    }

    vector<string> vec;
    vector<FixedComplex<32> > input; //Vector to hold answer data read from file
    std::string token;
    string line;

    while(getline(in,line)) {
       istringstream ss(line);
       getline(ss, token, ',');
       stringstream strValue;
       strValue << token;
       int realIntValue;
       int imagIntValue;
       strValue >> realIntValue;
       getline(ss, token, ',');
       stringstream strValue2;
       strValue2 << token;
       strValue2 >> imagIntValue;
       input.push_back(FixedComplex<32>(realIntValue, imagIntValue));
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    cout << "corrlate" << endl;
    vector<FixedComplex<32> > answers;
    XCorrelator x(16);
    answers = x.xCorrelate(input,input);

    for (int i = 0; i < answers.size(); i++) {
        cout << answers[i];
    }
}

CSIM_TEST_SUITE_END()
