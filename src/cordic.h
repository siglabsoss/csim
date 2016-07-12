/*
 * cordic.h
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#ifndef CORDIC_H_
#define CORDIC_H_

#include <iostream>
class cordic {
public:
	long double vals[30];
	int sign[30];
	long double k;

	long double calculate(long double theta, int a, int b);
	cordic();
	virtual ~cordic();
};

#endif /* CORDIC_H_ */
