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
#include "fixediir.h"
#define NUM_X_REGISTERS 2

#define NUM_Y_REGISTERS 5
//y = filter(b,a,dataIn)

FixedComplex<16> a[NUM_Y_REGISTERS];
FixedComplex<16> b[NUM_X_REGISTERS];//b coefficients
float temp1[NUM_Y_REGISTERS]  = {1,.5,.5,.6,.7}; // a coefficients
float temp2[NUM_X_REGISTERS] = {.5,.5};//b coefficients


using namespace boost;
using namespace std;


BOOST_AUTO_TEST_CASE(REAL_FILTER)
{

	for (int i = 0; i < NUM_Y_REGISTERS; i++)
		a[i].real = (temp1[i] * 32768);//Scales coefficients a

	for (int i = 0; i < NUM_X_REGISTERS; i++)
		b[i].real = (temp2[i] * 32768);//Scales coefficients b

	FixedComplex<16> output[1024];
	FixedComplex<16> input[1024];
	for (int i = 1; i < 5; i++)
	{
		input[i-1].real = ((double)i/10.00) * 32768;//Scales input data
	}


	fixediir iir(NUM_X_REGISTERS, NUM_Y_REGISTERS, a, b);
	iir.iir(input,output,4);

	BOOST_CHECK(abs(output[0].real/32768.00 - .05) < .001);
	BOOST_CHECK(abs(output[1].real/32768.00 - .125) < .001);
	BOOST_CHECK(abs(output[2].real/32768.00 - .1625) < .001);
	BOOST_CHECK(abs(output[3].real/32768.00 - .176250) < .001);
}


BOOST_AUTO_TEST_CASE(COMPLEX_FILTER)
{


    FixedComplex<16> input[1024];//Array to hold inputs
    FixedComplex<16> output[1024];//Array to hold outputs
    double realAnswers[1024];
    double imagAnswers[1024];
    FixedComplex<16> atap[100];
    FixedComplex<16> btap[100];

    string data("./data/iirdata/input/data1_in.csv");//Input data file

    ifstream in(data.c_str());
    if (!in.is_open())
    	cout << "error reading" << endl;
    char ** ptr;

    typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    vector< string > vec;
    string line;
    int i = 0;
    while (getline(in,line))
    {
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());
        input[i].real = atof(vec[0].c_str()) * 32768;
        input[i].imag = atof(vec[1].c_str()) * 32768;
     //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        i++;
    }//Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    string taps("./data/iirdata/input/ataps.csv");

    ifstream in2(taps.c_str());
    if (!in2.is_open())
    	cout << "error reading" << endl;

   typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    vector< string > vec2;
    string line2;
    int j = 0;

    while (getline(in2,line2))
    {

        Tokenizer tok(line2);
        vec2.assign(tok.begin(),tok.end());
        atap[j].real = atof(vec2[0].c_str()) * 32768;

        // cout<< setprecision(30)<< j+1 << ": Tap: " << tap[j].real.to_int() << " " << atof(vec2[0].c_str()) << " Is actually " << vec2[0].c_str() << endl;
        j++;
    }//Reads in taps



    string taps2("./data/iirdata/input/btaps.csv");

    ifstream in4(taps2.c_str());
    if (!in4.is_open())
    	cout << "error reading" << endl;

   typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    vector< string > vec4;
    string line4;
    int m = 0;

    while (getline(in4,line4))
    {

        Tokenizer tok(line4);
        vec4.assign(tok.begin(),tok.end());
        btap[m].real = atof(vec4[0].c_str()) * 32768;

        // cout<< setprecision(30)<< j+1 << ": Tap: " << tap[j].real.to_int() << " " << atof(vec2[0].c_str()) << " Is actually " << vec2[0].c_str() << endl;
        m++;
    }//Reads in taps

    string data3("./data/iirdata/answers/answers1.csv");//Answers data file

    ifstream in3(data3.c_str());
    if (!in3.is_open())
    	cout << "error reading" << endl;

    int l= 0;
    while (getline(in3,line))
	{
    	Tokenizer tok(line);
    	vec.assign(tok.begin(),tok.end());
    	realAnswers[l] = atof(vec[0].c_str());
    	imagAnswers[l] = atof(vec[1].c_str());
   	     //  cout << setprecision(30) << l+1 << ": Real: " << answers[i].real.to_int() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
    	l++;
	}//Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.


    fixediir iir(j, j, atap, btap);
	iir.iir(input,output,i);

	for (int k = 0; k < i; k++)
	{
		BOOST_CHECK_MESSAGE(abs(output[k].real/32768.00 -  realAnswers[k] < .001) , output[k].real/32768.00 << " is not the same as " << realAnswers[k]);
		BOOST_CHECK_MESSAGE(abs(output[k].imag/32768.00 -  imagAnswers[k] < .001) , output[k].imag/32768.00 << " is not the same as " << imagAnswers[k]);
		cout << output[k].real/32768.00 << " is the same as " << realAnswers[k] << endl;
	}




}
