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

#include "cic.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(REAL_FILTER) //Same as imaginary because there are only ads and subtracts
{

    FixedComplex<16> input[2048]; //Array to hold inputs
    FixedComplex<16> output[2048]; //Array to hold outputs
    FixedComplex<16> answers[2048];
    string data("./data/cicdata/cic_data_in.csv"); //Input data file

    ifstream in(data.c_str());
    if (!in.is_open())
        cout << "error reading" << endl;
    char ** ptr;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
    int i = 0;
    while (getline(in, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input[i].real = atof(vec[0].c_str());
        input[i].imag = atof(vec[1].c_str());
        //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    string data3("./data/cicdata/cic_data_out.csv"); //Answers data file

    ifstream in3(data3.c_str());
    if (!in3.is_open())
        cout << "error reading" << endl;

    int l = 0;
    while (getline(in3, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        answers[l].real = atof(vec[0].c_str());
        answers[l].imag = atof(vec[1].c_str());

        //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        l++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    fixedcic cic(2, 2, 2);
    cic.cic(i, input, output);

    for (int k = 0; k < i; k++) {
        // cout << k << endl;
        BOOST_CHECK_MESSAGE(abs(output[k].real - answers[k].real) < 1,
                output[k].real << " is not the same as " << answers[k].real << " " << k);
    } //Compares all outputs with solution to ensure they are .001 within each other.

}
