/*
 * twiddler.h

 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include <iostream>
#include <cmath>
#include "fixedcomplex.h"

#ifndef TWIDDLER_H_
#define TWIDDLER_H_

const float pi = 3.14159265359;
const int a = pow(2,15);

SC_MODULE (twiddler)
{
	sc_in <sc_int<32> > N, k;
	sc_out <sc_int<32> > W_r,W_im;
	float W_cos, W_sin, theta;
	int scale;
	void twiddler_comp ();

	SC_CTOR (twiddler)
	{
		SC_METHOD (twiddler_comp);
		sensitive (k);
	}
};




#endif /* TWIDDLER_H_ */
