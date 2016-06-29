/*
 * fixediir.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */

#include <fixediir.h>

fixediir::fixediir(int registerSize, FixedComplex<16>* aCoeffs, FixedComplex<16>* bCoeffs)
{

	this->numRegisters = registerSize; //number of coefficients of each side
	this->a = new FixedComplex<16>[this->numRegisters];
	this->b = new FixedComplex<16>[this->numRegisters];
	this->x = new FixedComplex<16>[this->numRegisters];
	this->y = new FixedComplex<16>[this->numRegisters];

	for (int i = 0; i < this->numRegisters; i++)
	{
		this->a[i] = aCoeffs[i];
		this->b[i] = bCoeffs[i];
	};//Gets coefficient data

}

void fixediir::iir(FixedComplex<16> *input, FixedComplex<16> *output, int inputs)
{

	for (int i = 0; i < this->numRegisters; i++)
	{

		x[i].real = 0;
		x[i].imag = 0;
		y[i].real = 0;
		y[i].imag = 0;
	}//Initialize registers

	for(int j=0; j < inputs; j++)
		output[j] = calculate(input[j]); //Calculate filtered data

}//Filters input data and places filtered data in output.



FixedComplex<16> fixediir::calculate(FixedComplex<16> current)
{
	 FixedComplex<32> currenty, CenterTap;//32 bits due to multiplication of 2 16 bits

	 CenterTap = (current.to_32() * b[0].to_32()); // x[0] * b[0]
	 for (int i = 1; i < numRegisters; i++)
		 CenterTap = CenterTap + ( x[i].to_32() * b[i].to_32());//Accumulate for each x register

	 CenterTap = CenterTap >> 15;//Reset to 16 bits
	 currenty = (CenterTap); // Coefficient of a[0] = 1
	 for (int i = 1; i < numRegisters; i++)
		 currenty = (currenty << 15)- (a[i].to_32() * y[i].to_32());//Accumulate

	 currenty = currenty >> 15;//Reset to 16 bits
	 for (int i = numRegisters - 1; i > 1; i--)
	 {
		 x[i] = x[i-1];
		 y[i] = y[i-1];
	 }//Moves everything up one register

	 x[1] = current;//Moves x[0] to x[1]
	 y[1] = currenty.to_16();//Moves y[0] to y[1]

	 return currenty.to_16();
}//Performs filtering

fixediir::~fixediir()
{

}

