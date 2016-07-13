/*
 * cordic.h
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#ifndef CORDIC_H_
#define CORDIC_H_

#include <iostream>
#include "fixedcomplex.h"
class cordic {
public:

	cordic();
	void rotate(sc_int<20> theta);//Calculates which direction to rotate
	void calculate(sc_int<20>theta, FixedComplex<16> a, FixedComplex<16> b, FixedComplex<32>* sin, FixedComplex<32>* cos);//Calculates sine and cosine
	void signs(); //swaps cosine and sign or changes sign if necessary
	virtual ~cordic();

	int vals[16];//Holds rotation values
	int sign[16];//Holds which direction to rotate
	FixedComplex<32>* sin;
	FixedComplex<32>* cos;
	int quad;
	double k;//Constant
	FixedComplex<32> y[2][1];//right aray to multiply (2x1)
	FixedComplex<32> z[2][2];//left matrixto multiply by(2x2)
	FixedComplex<32> temp;//temporary storage for later calculation
	FixedComplex<32> c1;//a-(1/2^n)b
	FixedComplex<32> c2;//a(1/2^n)+b

};

#endif /* CORDIC_H_ */
