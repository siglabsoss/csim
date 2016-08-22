#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <utils/utils.hpp>
#include <filters/fixed_fir.hpp>

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{

    vector<FixedComplex16> input; //Array to hold inputs
    vector<FixedComplex16> output; //Array to hold outputs
    vector<FixedComplex16>  tap;

    string inFile(argv[1]);
	string outFile(argv[2]);
	string tapsFile(argv[3]);

	input = complexRead16Scaled(inFile);
	if (input.empty()) {
		cout << "Could not read from " << inFile << endl;
		return 1;
	}

	tap = complexRead16Scaled(tapsFile);
	if (tap.empty()) {
		cout << "Could not read from " << tapsFile;
		return 1;
	}

    FixedFIR fir(tap); //Creates instance of fixed FIR filter given j taps.
    for (int k = 0; k < input.size(); k++) {
        filter_io_t data;
        data = input[k];
        fir.input(data); //Filters data
        filter_io_t output_sample;
        fir.output(output_sample);
        output.push_back(output_sample.fcn);
     }

    ofstream out(outFile);
    if (!out.is_open()) {
    	cout << "Could not write to " << outFile << endl;
    	return 1;
    }

    for (int k = 0; k < output.size(); k++) {
    	out << setw(6) << setfill(' ') <<  output[k].real().range().to_int64() << ",";
    	out << setw(6) << setfill(' ') <<  output[k].imag().range().to_int64() << endl;
    } //Prints all outputs

}
