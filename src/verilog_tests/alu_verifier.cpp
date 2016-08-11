#include<cstring>
#include <fstream>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <types/fixedcomplex.hpp>
#include <string>

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{
    FixedComplex<16> input1[1024]; //Array to hold inputs
    FixedComplex<16> input2[1024]; //Array to hold inputs
    string data(argv[1]); //Input data file
   // data = data + argv[1] + ".csv";
    cout << data << endl;
    ifstream in(data.c_str());
    if (!in.is_open()) {
        cout << "error reading" << data <<  endl;
        return 1;
    }
    char ** ptr;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;

    vector<string> vec;
    string line;
    string outFile(argv[2]);
    //outFile = outFile +  argv[1] + ".csv";
    ofstream out(outFile.c_str());
    if (!out.is_open()) {
          cout << "error reading" << outFile <<  endl;
          return 1;
      }
    int i = 0;
    while (getline(in, line)) {
       
        Tokenizer tok(line);
        vec.assign(tok.begin(), tok.end());
        input1[0].real = (vec[0].c_str());
        input1[0].imag = (vec[1].c_str());
        input2[0].real = (vec[2].c_str());
        input2[0].imag = (vec[3].c_str());
      
        out << (input1[0] + input2[0]).real << " "
                << (input1[0] + input2[0]).imag << " ";
        out << (input1[0] - input2[0]).real << " "
                << (input1[0] - input2[0]).imag << " ";
        out << (input1[0] * input2[0]).real << " "
                << (input1[0] * input2[0]).imag << " " << endl;
        //  out << (input1[i] + input2[i]).real << "," << (input1[i] + input2[i]).imag;
        i++;
    } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    in.close();
    out.close();

    return 0;
}
