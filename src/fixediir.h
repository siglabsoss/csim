/*
 * fixediir.h
 *
 *  Created on: Jun 28, 2016
 *      Author: ubuntu
 */

#ifndef FIXEDIIR_H_
#define FIXEDIIR_H_

class fixediir {
public:
	int numRegisters;
	double* a;
	double* b;
	double* x; //x registers
	double* y; //y registers

	fixediir(int registerSize, double* aCoeffs, double* bCoeffs);
	void iir(double *input, double *output, int inputs);
	double calculate(double current);
	virtual ~fixediir();
};

#endif /* FIXEDIIR_H_ */
