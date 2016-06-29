/*
 * fixediir.h
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */

#ifndef FIXEDIIR_H_
#define FIXEDIIR_H_
#include "fixedcomplex.h"


//Note that a[0] must be 1

class fixediir {
public:
	int numRegisters;
	FixedComplex<16>* a;//a coefficients
	FixedComplex<16>* b;//b coefficients
	FixedComplex<16>* x; //x registers
	FixedComplex<16>* y; //y registers

	fixediir(int registerSize, FixedComplex<16>* aCoeffs, FixedComplex<16>* bCoeffs);//Constructor
	void iir(FixedComplex<16> *input, FixedComplex<16> *output, int inputs);//Filtering input data
	FixedComplex<16> calculate(FixedComplex<16> current); // Calculate output based on input
	virtual ~fixediir();
};

#endif /* FIXEDIIR_H_ */
