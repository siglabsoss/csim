/*
 * fixedfir.h
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#ifndef FIXEDFIR_H_
#define FIXEDFIR_H_
#include "fixedcomplex.h"

class fixedfir {
public:
	int n; // Number of taps
	FixedComplex<16>* vals;
	FixedComplex<16> *output;
	
	
	fixedfir(int N, FixedComplex<16>* val);//Takes n number of taps and values representing each tap
	
	void fir(FixedComplex<16> *coeffs, int filterLength );
	void firFixedInit();
	virtual ~fixedfir();
};

#endif /* FIXEDFIR_H_ */
