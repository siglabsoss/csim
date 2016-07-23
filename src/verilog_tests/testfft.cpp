
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


int main()
{
    string infile("data/fft/input/data_file_complex.csv");
    string outfile("data/fft/output/out1.txt");
    cout << "program start" << endl;
    int i;
    int realInput[8] = {5,6,8,-5,6,12,10,9}; // default values
    int imagInput[8] = {0};
    ifstream in(infile.c_str());

    if (!in.is_open()){
            cout << "error reading infile" << endl;
            assert(1 == 0);// "Could not open data/fft/input/input3.txt");
    }//If cannot read from file, return 1;

    std::string token;
    for (i = 0; i < 8; i++) {
        string line;
        getline(in,line);
        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        realInput[i] = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        imagInput[i] = intValue;
    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    FixedComplex<16> answers[8];//Array to store answers
    int count = 0; //How many outputs have been collected
    fixedfft fft(8); //8 point fft

    for (i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            block_io_t data;
            data.type =  IO_TYPE_FIXED_COMPLEX_16;
            data.fc = FixedComplex<16>(realInput[j],imagInput[j]);
            fft.input(data);
            bool test = fft.output(data);
            if (test) {
                answers[count++] = data.fc;
            }
        }
    }
/*
 * If you want bits to be reversed


     cout << "Hopefully correct:" << endl;
     FixedComplex<16> temp[8];
     for (i = 0; i < 8; i++) {
         temp[reverseBits(8, i)] = answers[i];
     }//Reformats data in correct order
*/

     ofstream out2(outfile.c_str());

     for (i = 0; i < 8; i++) {
         out2 << setw(11) << setfill(' ') <<  answers[i].real.to_int() <<"," ;
         out2 << setw(11) << setfill(' ') << answers[i].imag.to_int() << endl;
         cout << answers[i];
     }//Prints data out in correct order

    cout << "program end" << endl;

}

