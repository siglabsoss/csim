
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
#include <filters/oldfixedfft.hpp>

using namespace std;


int main(int argc, char *argv[])
{
    string infile(argv[1]);
    string outfile(argv[2]);

    cout << "program start" << endl;
    int i;
    int realInput[32769] = {0}; // default values
    int imagInput[32769] = {0};
    ifstream in(infile.c_str());

    if (!in.is_open()){
            cout << "error reading" << infile << endl;
            assert(1 == 0);// "Could not open data/fft/input/input3.txt");
    }//If cannot read from file, return 1;

    ofstream out2(outfile.c_str());
    if (!out2.is_open()){
               cout << "error reading " << outfile << endl;
               assert(1 == 0);// "Could not open data/fft/input/input3.txt");
       }//If cannot read from file, return 1;

    std::string token;
    string line;
    int inputs = 0;
    while(getline(in,line)) {

        istringstream ss(line);
        getline(ss, token, ',');
        stringstream strValue;
        strValue << token;
        int intValue;
        strValue >> intValue;
        realInput[inputs] = intValue;
        getline(ss, token, ',');
        stringstream strValue2;
        strValue2 << token;
        strValue2 >> intValue;
        imagInput[inputs++] = intValue;

    }//Reads in inputs from file. Parsing by commas. Format is: real,imag\n

    FixedComplex<32> answers[32769];//Array to store answers
    int count = 0; //How many outputs have been collected

    int points = inputs;
    cout << inputs << endl;
    filter_io_t data;
    data.type =  IO_TYPE_FIXED_COMPLEX_32;


    oldfixedfft fft(points, 23040); //x point fft, y table size




    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < points; j++) {

            data.fc32 = FixedComplex<32>(realInput[j],imagInput[j]);
            fft.input(data);
            bool test = fft.output(data);
            if (test) {
                answers[count++] = data.fc32;
            }
        }
    }
    cout << "Count is: " << count << endl << endl;

    assert(count == inputs);
//
//     cout << "Hopefully correct:" << endl;
//     FixedComplex<32> temp[32769];
//    for (i = 0; i < inputs; i++) {
//        temp[reverseBits(inputs, i)] = answers[i];
//    }//Reformats data in correct order
//
//     for (i = 0; i < count; i++) {
//         out2 << setw(11) << setfill(' ') <<  temp[i].real.to_int() <<"," ;
//         out2 << setw(11) << setfill(' ') << temp[i].imag.to_int() << endl;
//       //  cout << temp[i];
//     }//Prints data out in correct order


     string outfile3("../data/fft/output/out1BitReversedold.txt");
     ofstream out3(outfile3.c_str());
     for (i = 0; i < count; i++) {
              out3 << setw(11) << setfill(' ') <<  answers[i].real.to_int() <<"," ;
              out3 << setw(11) << setfill(' ') << answers[i].imag.to_int() << endl;
            //  cout << temp[i];
          }//Prints data out in bit reversed order


    cout << "program end" << endl;


}

