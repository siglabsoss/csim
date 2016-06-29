#include "fixedfir.h"
#include "plotter.h"

using namespace boost;
using namespace std;

#define SAMPLES   1024

int main()
{
    FixedComplex<16> input[SAMPLES];//Array to hold inputs
    FixedComplex<16> output[SAMPLES];//Array to hold outputs

    string data("data/firdata/basic1_in.csv");//Input data file

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
    }//Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

    plotter plot = plotter();
    plot.some(1);
    plot.some(1.f);
    plot.some('c');
//    plot.nplot(input, "");

    vector<int > cvector;
    	for (int i = 0; i < 100; i++)
    	{
//    			complex<double> c(i, i*3);
    			cvector.push_back(i);
    	}

//    	 plot.nplot(cvector, "title");

    	 cout << "here" << endl;

    string taps("data/firdata/basic1_taps.csv");
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
    }//Reads in taps


    fixedfir fir(j, tap);//Creates instance of fixed FIR filter given j taps.
    fir.fir(i, input,output);//Filters data

    for (int k = 0; k < i; k++)
    	cout << k+1 << setprecision(5) << " Real: " << output[k].real/32768.00 << " Imaginary: " << output[k].imag/32768.00 << endl;//Prints out filtered data


    return 0;
}





