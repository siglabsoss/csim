/*
 * fixediir.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */

#include <fixediir.h>

fixediir::fixediir(int registerXSize, int registerYSize, FixedComplex<16>* aCoeffs, FixedComplex<16>* bCoeffs)
{

	this->numXRegisters = registerXSize; //number of coefficients on left side
	this->numYRegisters = registerYSize; //number of coefficients on right side
	this->a = new FixedComplex<16>[this->numYRegisters];
	this->b = new FixedComplex<16>[this->numXRegisters];
	this->x = new FixedComplex<16>[this->numXRegisters];
	this->y = new FixedComplex<16>[this->numYRegisters];

	for (int i = 0; i < this->numYRegisters; i++)
		this->a[i] = aCoeffs[i];//Gets coefficient data for right side

	for (int i = 0; i < this->numXRegisters; i++)
		this->b[i] = bCoeffs[i];//Gets coefficient data for left side

}

void fixediir::iir(FixedComplex<16> *input, FixedComplex<16> *output, int inputs)
{

	for (int i = 0; i < this->numXRegisters; i++)
	{
		x[i].real = 0;
		x[i].imag = 0;

	}//Initialize registers

	for (int i = 0; i < this->numYRegisters; i++)
	{
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

	 for (int i = 1; i < numXRegisters; i++)
		 CenterTap = CenterTap + ( x[i].to_32() * b[i].to_32());//Accumulate for each x register

	 CenterTap = CenterTap >> 15;//Reset to 16 bits

	 currenty = (CenterTap << 15 ); // Coefficient of a[0] = 1
	 for (int i = 1; i < numYRegisters; i++)
	 {
		 currenty = (currenty )- (a[i].to_32() * y[i].to_32());//Accumulate

	 }

	 currenty = currenty >> 15;//Reset to 16 bits

	 for (int i = numXRegisters - 1; i > 1; i--)
		 x[i] = x[i-1];//Moves everything up one register

	 for (int i = numYRegisters - 1; i > 1; i--)
		 y[i] = y[i-1];//Moves everything up one register

	 x[1] = current;//Moves x[0] to x[1]
	 y[1] = currenty.to_16();//Moves y[0] to y[1]

	 return currenty.to_16();
}//Performs filtering

fixediir::~fixediir()
{

}

