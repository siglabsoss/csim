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

#include <filters/fixed_fir.hpp>

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{
    FixedComplex16 input[1024]; //Array to hold inputs
    FixedComplex16 output[1024]; //Array to hold outputs
    double realAnswers[1024];
    double imagAnswers[1024];
    FixedComplex16 tap[100];
    FixedComplex16 tempTap[100];

    string infile(argv[1]);
	string outfile(argv[2]);
	string tapsFile(argv[3]);

	cout << "program start" << endl;
	int realInput[32769] = {0}; // default values
	int imagInput[32769] = {0};

	ifstream in(infile.c_str());
	if (!in.is_open()){
		cout << "error reading input " << infile << endl;
		return 1;
	}//If cannot read from file, return 1;

	ofstream out(outfile.c_str());
	if (!out.is_open()){
		cout << "error reading output " << outfile << endl;
		return 1;;
	}//If cannot read from file, return 1;

	ifstream taps(tapsFile.c_str());
	if (!taps.is_open()){
		cout << "error reading taps " << taps << endl;
		return 1;
	}//If cannot read from file, return 1;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
    int i = 0;
    while (getline(in, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        double real = atof(vec[0].c_str());
        double imag = atof(vec[1].c_str());
        input[i].real(real/32768.0);
        input[i].imag(imag/32768.0);
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;
    int j = 0; //number of taps

    while (getline(taps, line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        tap[j].real(atof(vec[0].c_str())/32768.0);
        j++;
    } //Reads in taps



    FixedFIR fir(j, tap); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < i; k++) {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output[k] = output_sample.fcn;
     }

    for (int k = 0; k < i; k++) {
    	out << setw(6) << setfill(' ') <<  output[k].real().range().to_int64() << ",";
    	out << setw(6) << setfill(' ') <<  output[k].imag().range().to_int64() << endl;
    } //Prints all outputs

}
