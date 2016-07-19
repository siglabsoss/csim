
#include "fixedfir.h"
#include "plotter.h"

using namespace boost;
using namespace std;

#define SAMPLES   1024

int main()
{


    FixedComplex<16> input[1024];//Array to hold inputs
    FixedComplex<16> output[1024];//Array to hold outputs
    FixedComplex<16> answers[1024];
    string data("./data/firdata/data2_in.csv");//Input data file

    ifstream in(data.c_str());
    if (!in.is_open())
    	cout << "error reading" << endl;
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
    }//Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    string taps("./data/firdata/data1_taps.csv");
    FixedComplex<16> tap[41];

    ifstream in2(taps.c_str());
    if (!in2.is_open())
    	cout << "error reading" << endl;

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
    }//Reads in taps



    fixedfir fir(j, tap);//Creates instance of fixed FIR filter given j taps.
    fir.fir(i, input,output);//Filters data

/*
 * Actual Plotting below
 *
 */


    plotter plot = plotter();

    std::vector<FixedComplex<16> > fcvector2;
    FixedComplex<16> fc2;
    for (int m = 0; m < i; m++)
    {
    	cout << m+1 << " Real: " << fc2.real.to_int()/32768.00 << "Imag: " << fc2.imag.to_int()/32768.00 << endl;
    	fc2.real = output[m].real;
    	fc2.imag = output[m].imag;
   		fcvector2.push_back(fc2);
    }//Places fir filtered data of FixedComplex into a vector

    plot.nplotqam(fcvector2, "fcvector2");//Plots fir filtered data

    vector<int> ivector;
    for (int i = 0; i < 100; i++)
      	ivector.push_back(i);
     plot.nplot(ivector, "int: nplot");//Plots vector of ints

    vector<sc_int<3> > scvector;
    sc_int<3> c;
    for (int i = 0; i < 6; i++)
    {
    	c = i;
    	scvector.push_back(c);
    }

    plot.nplot(scvector, "sc_int: nplot");//Plots vector of sc_int<3>
    plot.nplotfft(scvector, "sc_int: nplotfft");//plots vector of sc_int<3>



    return 0;
}


