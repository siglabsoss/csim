#include "cordic.h"
#include <iostream>



using namespace std;

int main()
{
    string outFile = "data/output/sinwave.csv";
    ofstream out(outFile.c_str());

	cordic c;
	FixedComplex<16> a(1,0);
	FixedComplex<16> b(0,0);
	FixedComplex<32> sin;
	FixedComplex<32> cos;

	for (float i = 0; i < 12.57; i = i + .07)
	{
		sc_int<20> j = i * 32768;
		c.calculate(j,a,b,&sin,&cos);
		cout << "Cosine: " << cos.real/32768.0 << endl;
		cout << "Sine: " << sin.real/32768.0 << endl;
		out << cos.real/32768.0<<",";
		out << sin.real/32768.0 << endl;
	}

	out.close();

}
