#include <mathlib/cordic.hpp>
#include <iostream>
#include <iomanip>
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <types/fixedcomplex.hpp>
#include <string>

using namespace std;
using namespace boost;

int main(int argc, char *argv[])
{
    string data(argv[1]); //Input data file
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
    ofstream out(outFile.c_str());
    if (!out.is_open()) {
             cout << "error reading" << outFile <<  endl;
             return 1;
         }
       int i = 0;
       cordic c;
       FixedComplex<16> a(1, 0);
       FixedComplex<16> b(0, 0);

       sc_int<32> cosdown;
       sc_int<32> cosup;
       sc_int<32> sinup;
       sc_int<32> sindown;
       int n = 0;
       int k = 0;
       while (getline(in, line)) {
           //cout << line << endl;
           Tokenizer tok(line);
           vec.assign(tok.begin(), tok.end());
           n = atoi(vec[0].c_str());
           k = atoi(vec[1].c_str());
          // cout << "N:" << n << " K:" << k<< endl;
           sc_int<20> j = (k * 32768/n) * 2 * 3.14159;
           cout << j << endl;
           c.calculate(j, a, b, &sinup, &sindown, &cosup, &cosdown);
           out << setw(11) <<  int((cosdown/32768.0) * 32)<< ",";
           out << setw(11) << int((sinup/32768.0) * 32) << endl;
       } //Gets each line of data. Stores real and imaginary parts separate in FixedComplex. i stores total number of inputs.

       in.close();
       out.close();
       return 0;
}
