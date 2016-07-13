/*
 * cordic.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#include "cordic.h"
using namespace std;

cordic::cordic() {
	for (int i = 0; i < 16; i++)
	{

		if (i == 0)
			vals[i] = .785398  * (2 << 14);
		else if (i == 1)
			vals[i] = .463648 * (2 << 14);
		else if (i == 2)
			vals[i] = .244979 *(2 << 14);
		else if (i == 3)
			vals[i] = .124355 * (2 << 14);
		else if (i == 4 )
			vals[i] = .062419 * (2 << 14);
		else
			vals[i] = ((float)1/(2 << (i-1))) * (2 << 14);

	}

	k = .6859;

}



long double cordic::calculate(long double thet, FixedComplex<16> a, FixedComplex<16> b)
{
	int x = 0;
	double ratio = (thet/1.5708);
	int theta = thet * (2 << 14);
	for (int i = 0; i < 16; i++)
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

	FixedComplex<32> y[2][1];
	FixedComplex<32> z[2][2];
	FixedComplex<32> temp;
	FixedComplex<32> c1;
	FixedComplex<32> c2;
	y[0][0] = (a.to_32()<<15);
	y[1][0] = (b.to_32()<<15);


	for (int i = 0; i < 16; i++)
	{
		z[0][0].real = 2 << 14;
	//	cout << z[0][0];
		if (i == 0)
			z[0][1].real = -(2 << 14);
		else
			z[0][1].real = (((double)-1/(2 << (i-1))) * (2 << 14));
	//	cout << z[0][1];

		z[1][0].real = -z[0][1].real;
		z[1][1].real = 2 << 14;
		temp = y[0][0];
	//	cout << y[0][0];

		c2 = (((z[0][1]) * (y[1][0].real))  >> 15);

		c2.real = c2.real * sign[i];
		c1 = (((z[0][0] * y[0][0]) >> 15));

		y[0][0] =c1   + c2;
	//	cout << c2;
		//cout << y[0][0];


		c2 = ((z[1][0] * temp) >> 15);
		c2.real = c2.real * sign[i];
	//	cout << c2;
		c1 = ((z[1][1] * y[1][0]) >> 15);
	//	cout << c1;
		y[1][0] = c2 + c1;
	//	cout << y[1][0];
	}

	y[0][0].real = (y[0][0].real * ( .607252935 * (2 << 14)));
	y[1][0].real= (y[1][0].real * ( .607252935 * (2 << 14)));

	y[0][0] = y[0][0] >> 15;
	y[1][0] = y[1][0] >> 15;

	cout << "Cosine: " << y[0][0].real/32768.0 << endl;
	cout << "Sine: " << y[1][0].real/32768.0 << endl;
	return 3.0;

}

cordic::~cordic() {

}

