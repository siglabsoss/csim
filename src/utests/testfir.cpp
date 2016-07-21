#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Complex
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include<cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include "fixedfir.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(REAL_FILTER)
{
    FixedComplex<16> input[1024]; //Array to hold inputs
    FixedComplex<16> output[1024]; //Array to hold outputs
    FixedComplex<16> answers[1024];
    string data("./data/firdata/input/basic1_in.txt"); //Input data file

    ifstream in(data.c_str());
    if (!in.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "./data/firdata/input/basic1_in.txt");
    }

    char ** ptr;
    typedef tokenizer<escaped_list_separator<char> > Tokenizer;
    vector<string> vec;
    string line;
    int i = 0;
    while (getline(in, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input[i].real = atof(vec[0].c_str()) * 32768;
        input[i].imag = atof(vec[1].c_str()) * 32768;
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    string taps("./data/firdata/input/basic1_taps.txt");
    FixedComplex<16> tap[41];

    ifstream in2(taps.c_str());
    if (!in2.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "./data/firdata/input/basic1_taps.txt");
    }
    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec2;
    string line2;
    int j = 0;

    while (getline(in2, line2)) {

        Tokenizer tok(line2);
        vec2.assign(tok.begin(), tok.end());
        tap[j].real = atof(vec2[0].c_str()) * 32768;

        // cout<< setprecision(30)<< j+1 << ": Tap: " << tap[j].real.to_int() << " " << atof(vec2[0].c_str()) << " Is actually " << vec2[0].c_str() <<endl;
        j++;
    } //Reads in taps

    string data3("./data/firdata/answers/answers1.txt"); //Answers data file

    ifstream in3(data3.c_str());
    if (!in3.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from ./data/firdata/answers/answers1.txt");
    }
    int l = 0;
    while (getline(in3, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        answers[l].real = atof(vec[0].c_str()) * 32768;
        answers[l].imag = atof(vec[1].c_str()) * 32768;
        l++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    FilterChainElement* fil;
    fixedfir fir(j, tap, fil); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < i; k++)
    {
        block_io_t data;
        data.type =  IO_TYPE_FIXED_COMPLEX_16;
        data.fc = input[k];
        fir.input(data); //Filters data
        output[k] = fir.m_output;
    }
    for (int k = 0; k < i; k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real / 32768.00 - answers[k].real / 32768.00)
                        < .001,
                output[k].real/32768.00 << " is not the same as " << answers[k].real/32768.00);
    } //Compares all outputs with solution to ensure they are .001 within each other.

}

BOOST_AUTO_TEST_CASE(COMPLEX_FILTER)
{
    FixedComplex<16> input[1024]; //Array to hold inputs
    FixedComplex<16> output[1024]; //Array to hold outputs
    double realAnswers[1024];
    double imagAnswers[1024];
    FixedComplex<16> tap[100];

    string data("./data/firdata/input/data1_in.txt"); //Input data file

    ifstream in(data.c_str());
    if (!in.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from ./data/firdata/input/data1_in.txt");
    }
    char ** ptr;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
    int i = 0;
    while (getline(in, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input[i].real = atof(vec[0].c_str()) * 32768;
        input[i].imag = atof(vec[1].c_str()) * 32768;
        //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
    string taps("./data/firdata/input/taps.txt");

    ifstream in2(taps.c_str());
    if (!in2.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from ./data/firdata/input/taps.txt");
    }
    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    int j = 0; //number of taps

    while (getline(in2, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        tap[j].real = atof(vec[0].c_str()) * 32768;
        j++;
    } //Reads in taps

    string data3("./data/firdata/output/data1_out.txt"); //Answers data file

    ifstream in3(data3.c_str());
    if (!in3.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from ./data/firdata/output/data1_out.txt");
    }

    int l = 0;//Number of answers
    while (getline(in3, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        realAnswers[l] = atof(vec[0].c_str());
        imagAnswers[l] = atof(vec[1].c_str());
        l++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    FilterChainElement* fil;
    fixedfir fir(j, tap, fil); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < i; k++) {
        block_io_t data;
        data.type =  IO_TYPE_FIXED_COMPLEX_16;
        data.fc = input[k];
        fir.input(data); //Filters data
        output[k] = fir.m_output;
     }

    for (int k = 0; k < i; k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real / 32768.00 - realAnswers[k]) < .001,
                input[k].real/32768.00 << " is not the same as " << realAnswers[k]);
        BOOST_CHECK_MESSAGE(
                abs(output[k].imag / 32768.00 - imagAnswers[k]) < .001,
                output[k].imag/32768.00 << " is not the same as " << imagAnswers[k]);
     //   cout << input[k].real / 32768.00 << " is the same as "
      //          << realAnswers[k] << endl;
    } //Compares all outputs with solution to ensure they are .001 within each other.

}
