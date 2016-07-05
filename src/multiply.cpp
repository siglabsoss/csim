#include<cstring>
#include <fstream>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
using namespace boost;
using namespace std;


int main()
{

    string data("data/jenkins/test1.csv");//Input data file

    ifstream in(data.c_str());
    if (!in.is_open())
    	return 1;
    else
    	cout << "Found file..." << endl;
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    string outFile = "data/output/out1.csv";
    ofstream out(outFile.c_str());

    vector< string > vec;
    string line;
    int i = 0;
    int a,b;
    while (getline(in,line))
    {
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());
        a = atoi(vec[0].c_str());
        b = atoi(vec[1].c_str());
        out << setw(10) << setfill(' ') << a*b << endl;
     //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        i++;
    }//Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.
    in.close();
    out.close();

    }//Reads in taps
