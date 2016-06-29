#include "fixediir.h"
#include <iostream>
#include <iomanip>
using namespace std;


FixedComplex<16> a[2];
FixedComplex<16> b[2];//b coefficients
float temp1[2]  = {1,.5}; // a coefficients
float temp2[2] = {.7,.5};//b coefficients


int main()
{

	for (int i = 0; i < 2; i++)
	{
		a[i].real = (temp1[i] * 32768);
		b[i].real = (temp2[i] * 32768);
	}

	FixedComplex<16> output[1024];
	FixedComplex<16> input[1024];
	for (int i = 1; i < 7; i++)
	{
		input[i-1].real = ((double)i/10.00) * 32768;
	}


	fixediir iir(2, a, b);
	iir.iir(input,output,6);


	for (int k = 0; k < 6; k++)
		cout << k+1 << setprecision(5) << " Real: " << output[k].real/32768.00 << " Imaginary: " << output[k].imag/32768.00 << endl;//Prints out filtered data
	return 0;

}

