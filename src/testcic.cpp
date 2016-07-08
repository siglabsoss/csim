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


BOOST_AUTO_TEST_CASE(REAL_FILTER)
{

	fixedcic cic(2);
    FixedComplex<16> input[1024];//Array to hold inputs
    FixedComplex<16> output[1024];//Array to hold outputs

    FixedComplex<16> test;
    for (int i = 1; i < 6; i++)
    {
    	input[i].real = i;
    	input[i].imag = i;
    }
    cout << "Before: " << endl;
    for (int i = 0; i < 5; i++)
    {
    	cout  << input[i];
    }


    cic.cic(5,input,output);

    cout << "After: " << endl;
    for (int i = 0; i < 5; i++)
    {
    	cout << output[i];
    }


}

BOOST_AUTO_TEST_CASE(COMPLEX_FILTER)
{



}
