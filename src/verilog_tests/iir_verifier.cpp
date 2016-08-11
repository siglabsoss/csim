#include <iostream>
#include<cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>
#include <bitset>
#include <algorithm>

#include <filters/fixed_iir.hpp>

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{

	string data(argv[1]); //Input data file
	ifstream in(data.c_str());
	if (!in.is_open()) {
		cout << "Could not read from " << data;
	}

	string outfile(argv[2]);
	ofstream out(outfile.c_str());
	if (!out.is_open()) {
		cout << "Could not read from " << outfile;
	}

	string taps(argv[3]);
	ifstream in2(taps.c_str());
	if (!in2.is_open()) {
		cout << "Could not read from " << taps;
	}

	string taps2(argv[4]);
	ifstream in4(taps2.c_str());
	if (!in4.is_open()) {
		cout << "Could not read from " << taps2;
	}

	FixedComplex16 input[1024]; //Array to hold inputs
	FixedComplex16 output[1024]; //Array to hold outputs
	double realAnswers[1024]; //Array to hold answers
	double imagAnswers[1024]; //Arrayto hodl answers
	FixedComplex16 atap[100]; //Array for A taps
	FixedComplex16 btap[100];


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

	int j = 0;
	while (getline(in2, line)) {

		Tokenizer tok(line);
		vec.assign(tok.begin(), tok.end());
		atap[j].real(atof(vec[0].c_str()));
		j++;
	} //Reads in A taps

	int m = 0;
	while (getline(in4, line)) {
		Tokenizer tok(line);
		vec.assign(tok.begin(), tok.end());
		btap[m].real(atof(vec[0].c_str()));
		m++;
	} //Reads in B taps


	fixediir iir(j, m, atap, btap);
	filter_io_t data2;
	for (int j = 0; j < i; j ++) {
		data2 = input[j];
		iir.input(data2); //Filters data
		output[j] = iir.m_output;
	}

	for (int k = 0; k < i; k++) {
		cout << output[k].real().range().to_int64() << " " << output[k].imag().range().to_int64() << endl;
	}

}
