/*
 * cordic.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#include "cordic.h"
using namespace std;

cordic::cordic() {
	for (int i = 0; i < 30; i++)
	{

		if (i == 0)
			vals[i] = .785398;
		else if (i == 1)
			vals[i] = .463648;
		else if (i == 2)
			vals[i] = .244979;
		else if (i == 3)
			vals[i] = .124355;
		else if (i == 4 )
			vals[i] = .062419;
		else
			vals[i] = (long double)1/(2 << (i-1));
	}


	k = .6859;

}

long double cordic::calculate(long double theta, int a, int b)
{
	long double x = 0;

	for (int i = 0; i < 30; i++)
	{
		if (x < theta)
		{
			sign[i] =  1;
			x = x + vals[i];
		}
		else// ( x > theta)
		{
			sign[i] = -1;
			x = x - vals[i];
		}

	}

	long double y[2][1];
	long double z[2][2];
	long double temp;
	y[0][0] = (long double)a;
	y[1][0] = (long double)b;

	for (int i = 0; i < 30; i++)
	{
		z[0][0] = 1;

		if (i == 0)
			z[0][1] = -1;
		else
			z[0][1] = (long double)-1/(2 << (i-1));
		z[1][0] = -z[0][1];
		z[1][1] = 1;
		temp = y[0][0];
		y[0][0] = (z[0][0] * y[0][0]) + (z[0][1] * y[1][0] * sign[i]);
		y[1][0] = (z[1][0]  * sign[i] * temp) + (z[1][1] * y[1][0]);

	}

	y[0][0]*= .607252935;
	y[1][0]*= .607252935;

	cout << "Cosine: " << y[0][0] << endl;
	cout << "Sine: " << y[1][0] << endl;
	return y[0][0];

}

cordic::~cordic() {

}

