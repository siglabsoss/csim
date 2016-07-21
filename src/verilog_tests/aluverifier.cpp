#include<cstring>
#include <fstream>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include "fixedcomplex.h"

using namespace boost;
using namespace std;

int main()
{
    FixedComplex<16> input1[1024]; //Array to hold inputs
    FixedComplex<16> input2[1024]; //Array to hold inputs

    string data("data/alu/input/in1.csv"); //Input data file

    ifstream in(data.c_str());
    if (!in.is_open()) {
        cout << "error reading" << endl;
        return 1;
    }
    char ** ptr;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
    string outFile("data/alu/output/out1.csv");
    ofstream out(outFile.c_str());

    int i = 0;
    while (getline(in, line)) {
        cout << line << endl;
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input1[0].real = (vec[0].c_str());
        input1[0].imag = (vec[1].c_str());
        input2[0].real = (vec[2].c_str());
        input2[0].imag = (vec[3].c_str());
        cout << i << endl;
        out << (input1[0] + input2[0]).real << " "
                << (input1[0] + input2[0]).imag << " ";
        out << (input1[0] - input2[0]).real << " "
                << (input1[0] - input2[0]).imag << " ";
        out << (input1[0] * input2[0]).real << " "
                << (input1[0] * input2[0]).imag << " " << endl;
        //  out << (input1[i] + input2[i]).real << "," << (input1[i] + input2[i]).imag;
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

//    while (i < 10)
//    {
//    	out << (input1[i] + input2[i]).real << "," << (input1[i] + input2[i]).imag;
//    }
    in.close();
    out.close();

    return 0;
}
