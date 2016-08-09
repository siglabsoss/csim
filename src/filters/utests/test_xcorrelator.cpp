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

CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_64)
{

    string inFile("data/xcorrelator/input/data_file_complex1.csv");
    string answersFile("data/xcorrelator/answers/answers1.csv");
    ifstream in(inFile.c_str());
    if (!in.is_open()) {
       cout << "error reading" << endl;
       BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from " << inFile);
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

    vector<FixedComplex<32> > output;
    XCorrelator x(input.size());
    output = x.xCorrelate(input,input);


    ifstream ans(answersFile.c_str());
    if (!ans.is_open()) {
       cout << "error reading" << endl;
       BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from " << answersFile);
    }

    vector<FixedComplex<32> > answers;
    while(getline(ans,line)) {
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
       answers.push_back(FixedComplex<32>(realIntValue, imagIntValue));
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

    for (int i = 0; i < answers.size(); i++) {
        BOOST_CHECK_MESSAGE(abs((output[i].real - answers[i].real)/(float)answers[i].real) < .05 || abs(abs(output[i].real) - abs(answers[i].real)) < 100 ,
                "I: " << i << " Output: " << output[i].real << " Answer: " << answers[i].real << " Ratio: " << abs((output[i].real - answers[i].real)/(float)answers[i].real) );
        BOOST_CHECK_MESSAGE(abs((answers[i].imag - output[i].imag )/(float)answers[i].imag) < .05 || abs(abs(output[i].imag) - abs(answers[i].imag)) < 3,
                "I: " << i << " Output: " << output[i].imag << " Answer: " << answers[i].imag << " Ratio: " << abs((output[i].imag - answers[i].imag)/(float)answers[i].imag) );
    }
}


CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_128)
{
    string inFile("data/xcorrelator/input/data_file_complex2.csv");
    string answersFile("data/xcorrelator/answers/answers2.csv");
    ifstream in(inFile.c_str());
    if (!in.is_open()) {
       cout << "error reading" << endl;
       BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from " << inFile);
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

    vector<FixedComplex<32> > output;
    XCorrelator x(input.size());
    output = x.xCorrelate(input,input);

    ifstream ans(answersFile.c_str());
    BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not read from " << answersFile);


    vector<FixedComplex<32> > answers;
    while(getline(ans,line)) {
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
       answers.push_back(FixedComplex<32>(realIntValue, imagIntValue));
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

    for (int i = 0; i < answers.size(); i++) {
        BOOST_CHECK_MESSAGE(abs((output[i].real - answers[i].real)/(float)answers[i].real) < .05 || abs(abs(output[i].real) - abs(answers[i].real)) < 100 ,
                "I: " << i << " Output: " << output[i].real << " Answer: " << answers[i].real << "Ratio: " << abs((output[i].real - answers[i].real)/(float)answers[i].real) );
        BOOST_CHECK_MESSAGE(abs((answers[i].imag - output[i].imag )/(float)answers[i].imag) < .05 || abs(abs(output[i].imag) - abs(answers[i].imag)) < 3,
                "I: " << i << " Output: " << output[i].imag << " Answer: " << answers[i].imag << "Ratio: " << abs((output[i].imag - answers[i].imag)/(float)answers[i].imag) );
    }
}


CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_256)
{

    string inFile("data/xcorrelator/input/data_file_complex3.csv");
    string answersFile("data/xcorrelator/answers/answers3.csv");
    ifstream in(inFile.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << inFile);

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

    vector<FixedComplex<32> > output;
    XCorrelator x(input.size());
    output = x.xCorrelate(input,input);

    ifstream ans(answersFile.c_str());
    BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not read from " << answersFile);
    vector<FixedComplex<32> > answers;
    while(getline(ans,line)) {
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
       answers.push_back(FixedComplex<32>(realIntValue, imagIntValue));
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

    for (int i = 0; i < answers.size(); i++) {
        BOOST_CHECK_MESSAGE(abs((output[i].real - answers[i].real)/(float)answers[i].real) < .05 || abs(abs(output[i].real) - abs(answers[i].real)) < 100 ,
                "I: " << i << " Output: " << output[i].real << " Answer: " << answers[i].real << "Ratio: " << abs((output[i].real - answers[i].real)/(float)answers[i].real) );
        BOOST_CHECK_MESSAGE(abs((answers[i].imag - output[i].imag )/(float)answers[i].imag) < .05 || abs(abs(output[i].imag) - abs(answers[i].imag)) < 3,
                "I: " << i << " Output: " << output[i].imag << " Answer: " << answers[i].imag << "Ratio: " << abs((output[i].imag - answers[i].imag)/(float)answers[i].imag) );
    }
}

CSIM_TEST_CASE(AUTO_CROSS_CORRELATOR_OCTAVE_COMPARISON_512)
{

    string inFile("data/xcorrelator/input/data_file_complex4x.csv");
    string answersFile("data/xcorrelator/answers/answers4x.csv");
    ifstream in(inFile.c_str());

    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << inFile);


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

    vector<FixedComplex<32> > output;
    XCorrelator x(input.size());
    output = x.xCorrelate(input,input);


    ifstream ans(answersFile.c_str());
    BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not read from " << answersFile);

    vector<FixedComplex<32> > answers;
    while(getline(ans,line)) {
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
       answers.push_back(FixedComplex<32>(realIntValue, imagIntValue));
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );

    for (int i = 0; i < answers.size(); i++) {
        BOOST_CHECK_MESSAGE(abs((output[i].real - answers[i].real)/(float)answers[i].real) < .10 || abs(abs(output[i].real) - abs(answers[i].real)) < 100 ,
                "I: " << i << " Output: " << output[i].real << " Answer: " << answers[i].real << "Ratio: " << abs((output[i].real - answers[i].real)/(float)answers[i].real) );
        BOOST_CHECK_MESSAGE(abs((answers[i].imag - output[i].imag )/(float)answers[i].imag) < .10 || abs(abs(output[i].imag) - abs(answers[i].imag)) < 3,
                "I: " << i << " Output: " << output[i].imag << " Answer: " << answers[i].imag << "Ratio: " << abs((output[i].imag - answers[i].imag)/(float)answers[i].imag) );
    }
}

CSIM_TEST_CASE(COMPLEX_CROSS_CORRELATOR_OCTAVE_COMPARISON_32)
{

    string inFile1("data/xcorrelator/input/data_file_complex5.csv");
    ifstream in1(inFile1.c_str());
    BOOST_REQUIRE_MESSAGE(in1.is_open(), "Could not read from " << inFile1);

    string inFile2("data/xcorrelator/input/data_file_complex6.csv");
    ifstream in2(inFile2.c_str());
    BOOST_REQUIRE_MESSAGE(in2.is_open(), "Could not read from " << inFile2);


    vector<string> vec;
    vector<FixedComplex<32> > input1; //Vector to hold answer data read from file
    vector<FixedComplex<32> > input2; //Vector to hold answer data read from file
    std::string token;
    string line;
    int size1 = 0;
    while(getline(in1,line)) {
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
       input1.push_back(FixedComplex<32>(realIntValue, imagIntValue));
       size1++;
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    int size2 = 0;
    while(getline(in2,line)) {
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
       input2.push_back(FixedComplex<32>(realIntValue, imagIntValue));
       size2++;
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    BOOST_REQUIRE_MESSAGE(size1 == size2, "Not equal amounts of inputs");
    vector<FixedComplex<32> > output;
    XCorrelator x(size1);
    output = x.xCorrelate(input1,input2);

    string answersFile("data/xcorrelator/answers/answers5.csv");
    ifstream ans(answersFile.c_str());

    BOOST_REQUIRE_MESSAGE(ans.is_open(), "Could not read from " << answersFile);


    vector<FixedComplex<32> > answers;
    while(getline(ans,line)) {
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
       answers.push_back(FixedComplex<32>(realIntValue, imagIntValue));
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    BOOST_REQUIRE_MESSAGE(answers.size() == output.size(), "There are " << output.size() << " outputs but there are " << answers.size() << " answers." );
    cout << output.size() << endl;
    for (int i = 0; i < answers.size(); i++) {
        BOOST_CHECK_MESSAGE(abs((output[i].real - answers[i].real)/(float)answers[i].real) < .05 || abs(abs(output[i].real) - abs(answers[i].real)) < 100 ,
                     "I: " << i << " Output: " << output[i].real << " Answer: " << answers[i].real << " Ratio: " << abs((output[i].real - answers[i].real)/(float)answers[i].real) );
             BOOST_CHECK_MESSAGE(abs((answers[i].imag - output[i].imag )/(float)answers[i].imag) < .05 || abs(abs(output[i].imag) - abs(answers[i].imag)) < 3,
                     "I: " << i << " Output: " << output[i].imag << " Answer: " << answers[i].imag << " Ratio: " << abs((output[i].imag - answers[i].imag)/(float)answers[i].imag) );

   }
}

CSIM_TEST_SUITE_END()
