#include <test/unit_test.hpp>

#include <iostream>
#include<cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include <filters/cic.hpp>

using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(CICFilter)

CSIM_TEST_CASE(REAL_FILTER) //Same as imaginary because there are only ads and subtracts
{
	cout << "STARTING CIC" << endl;
    FixedComplex2<16, 1> input[2048]; //Array to hold inputs
    FixedComplex2<16, 1> output[2048]; //Array to hold outputs
    FixedComplex2<16, 1> answers[2048];
    string data("./data/cicdata/input/cic_data_in.txt"); //Input data file

    ifstream in(data.c_str());
    if (!in.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from ./data/cicdata/input/cic_data_in.txt");
    }
    char ** ptr;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
    int i = 0;
    while (getline(in, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input[i].real(atof(vec[0].c_str()) / (1 << 15));

        //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    string data3("./data/cicdata/answers/answers1.txt"); //Answers data file
    ifstream in3(data3.c_str());
    if (!in3.is_open()) {
        cout << "error reading" << endl;
        BOOST_REQUIRE_MESSAGE(0 == 1, "Could not read from ./data/cicdata/answers/answers1.txt");
    }

    int l = 0;
    while (getline(in3, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        answers[l].real(atof(vec[0].c_str()) / (1 << 15));


        //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        l++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    fixedcic cic(2, 2, 2);
    int m = 0;
    for (int k = 0; k < i; k++)
      {
          filter_io_t data;
          data = input[k];
          cic.input(data); //Filters data
          bool test = cic.output(data);
          if (test) {
              output[m++] = data.fcn;
          }
}

    for (int k = 0; k < l; k++) {
        // cout << k << endl;
        BOOST_CHECK_MESSAGE(abs(output[k].real() - answers[k].real()) < 1,
                output[k].real() << " is not the same as " << answers[k].real() << " ");
    } //Compares all outputs with solution to ensure they are .001 within each other.

    cout << "DONE WITH CIC" << endl;
}

CSIM_TEST_SUITE_END()
