#include "fixediir.h"
#include <iostream>
#include <iomanip>

/*
 * Based on this design of an IIR filter: https://upload.wikimedia.org/wikipedia/commons/thumb/d/d5/IIR-filter.png/220px-IIR-filter.png
 *
 */
#define NUM_X_REGISTERS 2

#define NUM_Y_REGISTERS 5
using namespace std;


FixedComplex<16> a[NUM_Y_REGISTERS];
FixedComplex<16> b[NUM_X_REGISTERS];//b coefficients
float temp1[NUM_Y_REGISTERS]  = {1,.5,.5,.6,.7}; // a coefficients
float temp2[NUM_X_REGISTERS] = {.5,.5};//b coefficients


int main()
{

	for (int i = 0; i < NUM_Y_REGISTERS; i++)
		a[i].real = (temp1[i] * 32768);//Scales coefficients a

	for (int i = 0; i < NUM_X_REGISTERS; i++)
		b[i].real = (temp2[i] * 32768);//Scales coefficients b

	FixedComplex<16> output[1024];
	FixedComplex<16> input[1024];
	for (int i = 1; i < 5; i++)
	{
		input[i-1].real = ((double)i/10.00) * 32768;//Scales input data
		input[i-1].imag = ((double)(i+1)/10.00) * 32768;//Scales input data
	}


	fixediir iir(NUM_X_REGISTERS, NUM_Y_REGISTERS, a, b);
	iir.iir(input,output,4);


	for (int k = 0; k < 4; k++)
		cout << k+1 << setprecision(5) << " Real: " <<
			output[k].real/32768.00 << " Imaginary: " << output[k].imag/32768.00 << endl;//Prints out filtered data
	return 0;

}

