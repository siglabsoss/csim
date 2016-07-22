#include <mathlib/cordic.hpp>
#include <iostream>

using namespace std;

int main()
{
    string outFile = "data/cordic/output/sinwave.csv";
    ofstream out(outFile.c_str());

    string outFile2 = "data/cordic/output/clockup.csv";
    ofstream out2(outFile2.c_str());

    string outFile3 = "data/cordic/output/clockdown.csv";
    ofstream out3(outFile3.c_str());

    cordic c;
    FixedComplex<16> a(1, 0);
    FixedComplex<16> b(0, 1);
    sc_int<32> cosdown;
    sc_int<32> cosup;
    sc_int<32> sinup;
    sc_int<32> sindown;
    for (float i = 0; i < 12.57; i = i + .07) {
        sc_int<20> j = i * 32768;
        c.calculate(j, a, b, &sinup, &sindown, &cosup, &cosdown);
        //	cout << "Cosine: " << cosdown/32768.0 << endl;
        //	cout << "Sine: " << cosup/32768.0 << endl;
        //	out << cosdown/32768.0<<",";
//		out << cosup/32768.0 << endl;

        out2 << cosup / 32768.0 << ",";
        out2 << sinup / 32768.0 << endl;
        out3 << cosdown / 32768.0 << ",";
        out3 << sindown / 32768.0 << endl;
        sinup = c.sin(j);
        out << sinup / 32768.0 << endl;
    }

    out.close();

}
