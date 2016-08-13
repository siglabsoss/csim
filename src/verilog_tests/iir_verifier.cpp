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

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{

	vector<FixedComplex16> input; //Array to hold inputs
	vector<FixedComplex16> output; //Array to hold outputs
	double realAnswers[1024]; //Array to hold answers
	double imagAnswers[1024]; //Arrayto hodl answers
	vector<FixedComplex16> atap; //Array for A taps
	vector<FixedComplex16> btap;

	string inFile(argv[1]); //Input data file
	string outFile(argv[2]);
	string taps(argv[3]);
	string taps2(argv[4]);

	input = complexRead16Scaled(inFile);
	if (input.empty()) {
		cout << "Could not read from " << inFile << endl;
		return 1;
	}

	atap = complexRead16Scaled(taps);
	if (atap.empty()) {
		cout << "Could not read from " << taps << endl;
		return 1;
	}//Gets A taps

	btap = complexRead16Scaled(taps2);
	if (btap.empty()) {
		cout << "Could not read from " << taps2 << endl;
		return 1;
	}//Gets A taps

	fixediir iir(atap, btap);
	filter_io_t data2;
	for (int j = 0; j < input.size(); j ++) {
		data2 = input[j];
		iir.input(data2); //Filters data
		output[j] = iir.m_output;
	}

	ofstream out(outFile);
    if (!out.is_open()) {
    	cout << "Could not write to " << outFile << endl;
    	return 1;
    }
	for (int k = 0; k < output.size(); k++) {
    	out << setw(6) << setfill(' ') <<  output[k].real().range().to_int64() << ",";
    	out << setw(6) << setfill(' ') <<  output[k].imag().range().to_int64() << endl;
	}

}
