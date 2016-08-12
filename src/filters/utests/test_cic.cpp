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

#include <filters/fixed_cic.hpp>

using namespace boost;
using namespace std;

CSIM_TEST_SUITE_BEGIN(CICFilter)

CSIM_TEST_CASE(REAL_FILTER) //Same as imaginary because there are only adds and subtracts
{

    FixedComplex16 input[2048]; //Array to hold inputs
    FixedComplex16 output[2048]; //Array to hold outputs
    FixedComplex16 answers[2048];
    string data("./data/cic/input/cic_data_in.txt"); //Input data file

    ifstream in(data.c_str());
    BOOST_REQUIRE_MESSAGE(in.is_open(), "Could not read from " << data);

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

    string data3("./data/cic/answers/answers1.txt"); //Answers data file
    ifstream in3(data3.c_str());
    BOOST_REQUIRE_MESSAGE(in3.is_open(), "Could not read from " << data3);

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

}

CSIM_TEST_SUITE_END()
