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

#include "Stitcher.h"

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE(PYTHON_COMPARISON)
{
	int waves[2] = {1,0};
	int samples[2] = {200,200};
//	int waves[2] = {1,1};
//	int samples[2] = {50,50};
	int num_sections = sizeof(samples) / sizeof(samples[0]);
	vector<FixedComplex<32> > datas;
	for (int k = 0; k < 7 ; k++)
		for ( int i = 0; i < 32768; i++)
		{
			FixedComplex<32> j(i,i);
			datas.push_back(j);
		}

	Stitcher stitch(waves, samples, num_sections);//Creates stitcher
	vector<FixedComplex<32> > outdatas;//Vector for output data
	outdatas = stitch.stitch(400, 1000, 100, datas);//(int numsamples, int sampleRate, int frequency) stitches data with clockup and clockdown waves

	int size = outdatas.size();
	string outFile = "data/output/stitching2.csv";//Output data file
	ofstream out(outFile.c_str());

	for (int i = 0; i < size; i++)
	{
		out << outdatas[i].real/32768.0 <<"," << outdatas[i].imag/32768.0 << endl;
		cout << outdatas[i];
	}
	cout << "DONE" << endl;

	char ** ptr;
	typedef tokenizer< escaped_list_separator<char> > Tokenizer;

	vector<string> vec;
	vector<double> answers;//Vector to hold answer data read from file
	string line;
	string data2("data/output/stitcher_answers.csv");//Answers data file
	ifstream in2(data2.c_str());
	if (!in2.is_open())
	{
		cout << "error reading" << endl;
		return;
	}

	int l = 0;
	while (getline(in2,line))
	{
		Tokenizer tok(line);
	   	vec.assign(tok.begin(),tok.end());
	   	answers.push_back(atof(vec[0].c_str()));
	   	l++;
	}//Gets each line of data. Allows separation of real and imaginary in future. Currently only real.

	for(int i = 0;  i < 200; i++)
	{
		BOOST_CHECK_MESSAGE(abs(outdatas[i].real/32768.0 - answers[i]) < .005, i << ": " << outdatas[i].real/32768.0 << " is not equal to " << answers[i]);
	}//Checks output data and answers to verify they are within .005 of each other

}
