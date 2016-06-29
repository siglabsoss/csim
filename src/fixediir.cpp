/*
 * fixediir.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */

#include <fixediir.h>

fixediir::fixediir(int registerSize, double* aCoeffs, double* bCoeffs)
{

	this->numRegisters = registerSize; //number of coefficients of each side
	this->a = new double[this->numRegisters];
	this->b = new double[this->numRegisters];
	this->x = new double[this->numRegisters];
	this->y = new double[this->numRegisters];

	for (int i = 0; i < this->numRegisters; i++)
	{
		this->a[i] = aCoeffs[i];
		this->b[i] = bCoeffs[i];
	};//Gets coefficient data

}

void fixediir::iir(double *input, double *output, int inputs)
{

	for (int i = 0; i < this->numRegisters; i++)
	{
		x[i] = 0.0;
		y[i] = 0.0;
	}//Initialize registers

	for(int j=0; j < inputs; j++)
		output[j] = calculate(input[j]); //Calculate filtered data
}

double fixediir::calculate(double current)
{
	 double currenty, CenterTap;
	 CenterTap = current * b[0];

	 for (int i = 0; i < numRegisters; i++)
		 CenterTap += x[i] * b[i];//Accumulate

	 currenty = a[0] * CenterTap;
	 for (int i = 0; i < numRegisters; i++)
		 currenty -= a[i] * y[i];//Accumulate


	 for (int i = numRegisters - 1; i > 1; i--)
	 {
		 x[i] = x[i-1];
		 y[i] = y[i-1];
	 }//Moves everything up one register

	 x[1] = current;
	 y[1] = currenty;

	 return currenty;
}

fixediir::~fixediir() {
	// TODO Auto-generated destructor stub
}

