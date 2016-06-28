#include "fixedfir.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp>
#include <iomanip>

using namespace boost;
using namespace std;

#define SAMPLES   1024

int main()
{
	
    int size;
    FixedComplex<16> input[SAMPLES];
    FixedComplex<16> output[SAMPLES];
    FILE   *out_fid;


    // open the output waveform file
    out_fid = fopen( "outputFixed.pcm", "w" );
    if ( out_fid == 0 ) {
        printf("couldn't open outputFixed.pcm");
        exit(EXIT_FAILURE);
    }

    string data("../data/firdata/basic1_in.csv");

    ifstream in(data.c_str());
    if (!in.is_open())
    	return 1;
    char ** ptr;

    typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    vector< string > vec;
    string line;
    int i = 0;
    while (getline(in,line))
    {
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());
        input[i].real = atof(vec[0].c_str()) * 32768;
        input[i].imag = atof(vec[1].c_str()) * 32768;
     //   cout << setprecision(30) << i+1 << ": Real: " << input[i].real.to_int() << " " << atof(vec[0].c_str()) << " Is actually " << vec[0].c_str() << endl;//" Imaginary: "<< input[j].imag.to_int() << endl;
        i++;
    }

    string taps("../data/firdata/basic1_taps.csv");
    FixedComplex<16> tap[41];


       ifstream in2(taps.c_str());
       if (!in2.is_open())
       	return 1;


       typedef tokenizer< escaped_list_separator<char> > Tokenizer;

       vector< string > vec2;
       string line2;
       int j = 0;

       while (getline(in2,line2))
       {

           Tokenizer tok(line2);
           vec2.assign(tok.begin(),tok.end());
           tap[j].real = atof(vec2[0].c_str()) * 32768;

          // cout<< setprecision(30)<< j+1 << ": Tap: " << tap[j].real.to_int() << " " << atof(vec2[0].c_str()) << " Is actually " << vec2[0].c_str() <<endl;
           j++;

       }

    fixedfir fir(j, tap);

//    FixedComplex<16> input2[6];
//    FixedComplex<16> output2[6];
//    for (int i = 0;  i < 6; i++)
//    	input2[i].real = i+1;
//
//FixedComplex<16> tap2[4];
//for (int i = 0; i < 4; i ++)
//	tap2[i].real = 2;
//
//    fixedfir fir2(4, tap2);
//
//    fir2.fir(6, input2, output2);

    fir.fir(i, input,output);



    for (int k = 0; k < 10; k++)
    {
    	cout << k+1 << setprecision(5) << " Real: " << output[k].real/32768.000 <<endl;
    }

    fclose( out_fid );

    
    return 0;
}





