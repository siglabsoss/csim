/*
 * fixedfir.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#define FILTER_LEN  63

#ifndef FIXEDFIR_H_
#define FIXEDFIR_H_
#include "fixedcomplex.h"

class fixedfir {
public:
	

	int n; // Number of taps
	FixedComplex<16>* taps;
	FixedComplex<16> coeffs[ FILTER_LEN ];//Filter coeffs
	
	fixedfir(int N, FixedComplex<16>* val);

	void fir(FixedComplex<16>* output);
	void firFixedInit();
	
	virtual ~fixedfir();
	
	
};

#endif /* FIXEDFIR_H_ */
