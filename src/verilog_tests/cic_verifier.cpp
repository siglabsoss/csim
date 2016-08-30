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
#include <utils/utils.hpp>
using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{
	vector<FixedComplex16> input; //Array to hold inputs
	vector<FixedComplex16> output; //Array to hold outputs
	int settings[3] = {0};//Settings for decimation factor, a regs, and b regs

	string inFile(argv[1]); //Input data file
	string outFile(argv[2]); //Input data file

	input = complexRead16Scaled(inFile);
	if(input.empty()) {
	   cout << "Could not read from " << inFile; //Reads input file
	}//Verifies input was read

	string settingsFile(argv[3]); //Input data file
	ifstream set(settingsFile.c_str());
	if (!set.is_open()) {
		cout << "Could not read from " << settingsFile << endl;
		return 1;
	}//verifies settings file can be read

	string line;
	int num = 0; // Which setting is being set
	while (getline(set, line)) {
		settings[num] = stoi(line);
		cout << settings[num] << endl;
		num++;
	}//Get settings of cic

	fixedcic cic(settings[0], settings[1], settings[2]); //Constructor takes in decimation factor and number of regs on each side.
    int m = 0;
    for (unsigned int k = 0; k < input.size(); k++)
	{
    	filter_io_t data;
		data = input[k];
		cic.input(data); //Filters data
		bool test = cic.output(data);
		if (test) {
			output.push_back(data.fc);
		}//If output is available, add it to output vector
	}//For all inputs

    ofstream out(outFile);
    if (!out.is_open()){
    	cout << "Could not read from " << outFile << endl;
    	return 1;
    }//Verifies output file can be written to

	for (int k = 0; k < m; k++) {
		//	cout << output[k].real().range().to_int64() << " " << output[k].imag().range().to_int64() << endl;
	    	out << setw(6) << setfill(' ') <<  output[k].real().range().to_int64() << ",";
	    	out << setw(6) << setfill(' ') <<  output[k].imag().range().to_int64() << endl;
	}//Writes to file
}
