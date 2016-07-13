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
	int vals[16];
	int sign[16];
	long double k;
	void rotations(long double theta);
	long double calculate(long double theta, FixedComplex<16> a, FixedComplex<16> b);
	cordic();
	virtual ~cordic();
};

#endif /* CORDIC_H_ */
