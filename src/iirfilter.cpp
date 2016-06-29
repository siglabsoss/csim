#include "fixediir.h"
#include <iostream>
#include <iomanip>
#define NUM_REGISTERS 5
using namespace std;


FixedComplex<16> a[NUM_REGISTERS];
FixedComplex<16> b[NUM_REGISTERS];//b coefficients
float temp1[NUM_REGISTERS]  = {1,.5,.5,.6,.7}; // a coefficients
float temp2[NUM_REGISTERS] = {.5,.5,.5,.4,.3};//b coefficients


int main()
{

	for (int i = 0; i < NUM_REGISTERS; i++)
	{
		a[i].real = (temp1[i] * 32768);//Scales coefficients
		b[i].real = (temp2[i] * 32768);//Scales coefficients

	}

	FixedComplex<16> output[1024];
	FixedComplex<16> input[1024];
	for (int i = 1; i < 5; i++)
	{
		input[i-1].real = ((double)i/10.00) * 32768;//Scales input data
		input[i-1].imag = ((double)(i+1)/10.00) * 32768;//Scales input data
	}


	fixediir iir(NUM_REGISTERS, a, b);
	iir.iir(input,output,6);


	for (int k = 0; k < 4; k++)
		cout << k+1 << setprecision(5) << " Real: " << output[k].real/32768.00 << " Imaginary: " << output[k].imag/32768.00 << endl;//Prints out filtered data
	return 0;

}

