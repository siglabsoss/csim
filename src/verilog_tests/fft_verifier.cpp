
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>

#include <utils/utils.hpp> //reverseBits()
#include <filters/fixedfft.hpp>

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
	fixedfft fft(points,23040); //x point fft, y table size

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < points; j++) {
			data = inputs[j];
			fft.input(data);
			bool test = fft.output(data);
			if (test) {
				outputs.push_back(data.fcn32);
			}//If output is ready
		}//Insert all input
	}//Insert input again to get output

    assert(outputs.size() == inputs.size());
    ofstream out2(outFile.c_str());
	if (!out2.is_open()){
			   cout << "error reading " << outFile << endl;
			   assert(1 == 0);// "Could not open data/fft/input/input3.txt");
   }//If cannot read from file, return 1;

     for (unsigned int i = 0; i < outputs.size(); i++) {
              out2 << setw(11) << setfill(' ') <<  outputs[i].real().range().to_int64()   << ",";
              out2 << setw(11) << setfill(' ') <<  outputs[i].imag().range().to_int64()   << endl;
            //  cout << temp[i];
          }//Prints data out in bit reversed order

     cout << "Completed" << endl;
//
//     string outfile3("../data/fft/output/out1NotBitReversed.txt");
//         ofstream out3(outfile3.c_str());
//     cout << "Hopefully correct:" << endl;
//     FixedComplex<32> temp[32769];
//    for (i = 0; i < inputs; i++) {
//        temp[reverseBits(inputs, i)] = answers[i];
//    }//Reformats data in correct order
//
//     for (i = 0; i < count; i++) {
//         out3 << setw(11) << setfill(' ') <<  temp[i].real.to_int() <<"," ;
//         out3 << setw(11) << setfill(' ') << temp[i].imag.to_int() << endl;
//       //  cout << temp[i];
//     }//Prints data out in correct order
}

