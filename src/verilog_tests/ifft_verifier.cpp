
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>
#include <filters/fixedfft.cpp>
#include <utils/utils.hpp> //reverseBits()

using namespace std;


int main(int argc, char *argv[])
{
    string inFile(argv[1]);
    string outFile(argv[2]);

    vector<FixedComplex32> inputs = complexRead32Scaled(inFile);
    if (inputs.empty()){
            cout << "error reading" << inFile << endl;
            return 1;// "Could not open data/fft/input/input3.txt");
    }//If cannot read from file, return 1;

	std::vector<FixedComplex32> outputs;

	int points = inputs.size();
    filter_io_t data;
    fixedfft ifft(points, 23040, true); //8 point ifft

    for (int i = 0; i < 2; i++) {
    		for (int j = 0; j < points; j++) {
    			data = inputs[j];
    			ifft.input(data);
    			bool test = ifft.output(data);
    			if (test) {
    				outputs.push_back(data.fc);
    			}//If output is ready
    		}//Insert all input
    	}//Insert input again to get output

    assert(outputs.size() == inputs.size());
	ofstream out2(outFile.c_str());
	if (!out2.is_open()){
		cout << "error reading " << outFile << endl;
		assert(1 == 0);// "Could not open data/fft/input/input3.txt");
	}//If cannot read from file, return 1;

    for (unsigned int k = 0; k < outputs.size(); k++) {
        out2 << setw(11) << setfill(' ') <<  outputs[k].real().range().to_int64()   << ",";
        out2 << setw(11) << setfill(' ') <<  outputs[k].imag().range().to_int64()   << endl;
       // cout << answers[i];
    }//Prints data

//

    cout << "program end" << endl;


}

