#include <test/unit_test.hpp>

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>
#include <filters/fixed_iir.hpp>


#define NUM_X_REGISTERS 2
#define NUM_Y_REGISTERS 5

vector<FixedComplex16 > a (NUM_Y_REGISTERS);
vector<FixedComplex16> b(NUM_X_REGISTERS); //b coefficients
float temp1[NUM_Y_REGISTERS] = { 1, .5, .5, .6, .7 }; // a coefficients
float temp2[NUM_X_REGISTERS] = { .5, .5 }; //b coefficients

using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(IIRFilter)

CSIM_TEST_CASE(REAL_FILTER)
{

    for (int i = 0; i < NUM_Y_REGISTERS; i++) {
        a[i].real(temp1[i]);
    }

    for (int i = 0; i < NUM_X_REGISTERS; i++) {
        b[i].real(temp2[i]);
    }

    FixedComplex16 output[1024];
    FixedComplex16 input[1024];

    for (int i = 1; i < 5; i++) {
        input[i - 1].real((double) i / 10.00);
    }

    fixediir iir(a, b);

    filter_io_t data;
    for (int i = 0; i < 4; i ++) {


      data = input[i];
      iir.input(data); //Filters data
      output[i] = iir.m_output;
    }
    BOOST_CHECK(abs(output[0].real() - .05) < .001);
    BOOST_CHECK(abs(output[1].real() - .125) < .001);
    BOOST_CHECK(abs(output[2].real() - .1625) < .001);
    BOOST_CHECK(abs(output[3].real() - .176250) < .001);
}


CSIM_TEST_CASE(COMPLEX_FILTER)
{

    FixedComplex16 input[1024]; //Array to hold inputs
    FixedComplex16 output[1024]; //Array to hold outputs
    double realAnswers[1024]; //Array to hold answers
    double imagAnswers[1024]; //Arrayto hodl answers
    vector<FixedComplex16> atap(100); //Array for A taps
    vector<FixedComplex16> btap(100);

    string data("./data/iir/input/data1_in.csv"); //Input data file

    ifstream in(data.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << data);

    char ** ptr;
    typedef tokenizer<escaped_list_separator<char> > Tokenizer;
    vector<string> vec;
    string line;
    int i = 0; //Number of inputs
    while (getline(in, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input[i].real(atof(vec[0].c_str()));
        input[i].imag(atof(vec[1].c_str()));
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    string taps("./data/iir/input/ataps.txt");
    ifstream in2(taps.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << taps);
    int j = 0;
    while (getline(in2, line)) {

        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        atap[j].real(atof(vec[0].c_str()));
        j++;
    } //Reads in taps

    string taps2("./data/iir/input/btaps.txt");
    ifstream in4(taps2.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << taps2);

    int m = 0;
    while (getline(in4, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        btap[m].real(atof(vec[0].c_str()));
        m++;
    } //Reads in taps

    string data3("./data/iir/answers/answers1.txt"); //Answers data file
    ifstream in3(data3.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << data3);
    int l = 0;
    while (getline(in3, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        realAnswers[l] = atof(vec[0].c_str());
        imagAnswers[l] = atof(vec[1].c_str());
        l++;
    } //Gets each line of answers. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    assert(l == i);//Length of inputs is length of answers/outputs.
    fixediir iir(atap, btap);
    filter_io_t data2;
    for (int j = 0; j < i; j ++) {
        data2 = input[j];
        iir.input(data2); //Filters data
        output[j] = iir.m_output;
    }

    for (int k = 0; k < i; k++) {
        BOOST_CHECK_MESSAGE(
                abs(output[k].real() - realAnswers[k]) < .001,
                output[k].real() << " is not the same as " << realAnswers[k]);
        BOOST_CHECK_MESSAGE(
                abs(output[k].imag() - imagAnswers[k]) < .001,
                output[k].imag() << " is not the same as " << imagAnswers[k]);
//        cout << output[k].real / 32768.00 << " is the same as "
//                << realAnswers[k] << endl;
 //   	cout << output[k].real().range().to_int64() << " " << output[k].imag().range().to_int64() << endl;
    }



}

CSIM_TEST_SUITE_END()
