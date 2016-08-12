#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>
#include <bitset>
#include <string>

#include <filters/fixed_cic.hpp>

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{
	cout << "hi" << endl;
	vector<FixedComplex16> input; //Array to hold inputs
	vector<FixedComplex16> output; //Array to hold outputs
	int settings[3] = {0};//Settings for decimation factor, a regs, and b regs

	string inFile(argv[1]); //Input data file
	ifstream in(inFile.c_str());
	if (!in.is_open()) {
		cout << "Could not read from " << inFile << endl;
		return 1;
	}

	string outFile(argv[2]); //Input data file
	ofstream out(outFile.c_str());
	if (!out.is_open()) {
		cout << "Could not write to " << outFile << endl;
		return 1;
	}

	string settingsFile(argv[3]); //Input data file
	ifstream set(settingsFile.c_str());
	if (!set.is_open()) {
		cout << "Could not read from " << settingsFile << endl;
		return 1;
	}

	char ** ptr;
	typedef tokenizer<escaped_list_separator<char> > Tokenizer;
	vector<string> vec;
	string line;
	FixedComplex16 temp;
	while (getline(in, line)) {
		Tokenizer tok(line);
		vec.assign(tok.begin(), tok.end());
		temp.real(atof(vec[0].c_str())/32768.0);
		temp.imag(atof(vec[1].c_str())/32768.0);
		input.push_back(temp);
	} //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

	int num = 0; // Which setting is being set
	while (getline(set, line)) {
		settings[num] = stoi(line);
		cout << settings[num] << endl;
		num++;
	}


	fixedcic cic(settings[0], settings[1], settings[2]); //Constructor takes in decimation factor and number of regs on each side.

	int m = 0;//Number of outputs
	for (int k = 0; k < input.size(); k++)
	  {
		  filter_io_t data;
		  data = input[k];
		  cic.input(data); //Filters data
		  bool test = cic.output(data);
		  if (test) {
			  output.push_back(data.fcn);
			  m++;
	  }
	}

	for (int k = 0; k < m; k++) {
		//	cout << output[k].real().range().to_int64() << " " << output[k].imag().range().to_int64() << endl;
	    	out << setw(6) << setfill(' ') <<  output[k].real().range().to_int64() << ",";
	    	out << setw(6) << setfill(' ') <<  output[k].imag().range().to_int64() << endl;
		}
}
