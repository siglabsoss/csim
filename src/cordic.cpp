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

	}//Calculates and sets values to rotate by

	k = .607252935;//constant k

}
void cordic::rotate(sc_int<20> theta)
{
	while (theta >=  205888)//input greater than 2pi
		theta = theta - 205888;


	if (theta >= 154415)//4th quadrant
	{
		quad = 4;
		theta = theta - 154415;
	}
	else if (theta >= 102943)//3rd quadrant
	{
		quad = 3;
		theta = theta - 102943;
	}
	else if (theta >= 51471)//2nd quadrant
	{
		quad = 2;
		theta = theta - 51471;
	}
	else//1st quadrant
		quad = 1;


	sc_int<20> x = 0; //starting point for rotations
	//theta = theta * (1 << 15);//scale up theta
	for (int i = 0; i < 30; i++)
	{
		if (x < theta)
		{
			sign[i] =  1;
			x = x + vals[i];
		}//rotate ccw
		else// ( x > theta)
		{
			sign[i] = -1;
			x = x - vals[i];
		}//rotate cw
	}
}


void cordic::calculate(sc_int<20>theta, FixedComplex<16> a, FixedComplex<16> b, FixedComplex<32>* sin, FixedComplex<32>* cos)
{
	rotate(theta);

	y[0][0] = (a.to_32()<<15); //scale up input
	y[1][0] = (b.to_32()<<15); //scale up input

	for (int i = 0; i < 30; i++)
	{
		z[0][0].real = 1 << 15; //32768. Upper left

		if (i == 0)
			z[0][1].real = -(1 << 15); // First values of z[0][1] must be -1. Upper right
		else
			z[0][1].real = (((double)-1/(2 << (i-1))) * (1 << 15)); // 1/2^(i-1). Upper right

		z[1][0].real = -z[0][1].real; // Lower left
		z[1][1].real = 1 << 15; // 1. Lower right
		temp = y[0][0]; //temporary storage for later calculation

		c1 = (((z[0][0] * y[0][0]) >> 15));
		c2 = (((z[0][1]) * (y[1][0].real))  >> 15);
		c2.real = c2.real * sign[i];//multiply by rotation direction
		y[0][0] =c1   + c2;//Upper value

		c1 = ((z[1][1] * y[1][0]) >> 15);
		c2 = ((z[1][0] * temp) >> 15);
		c2.real = c2.real * sign[i]; //multiply by rotation direction
		y[1][0] = c2 + c1;//Lower value

	}

	y[0][0].real = (y[0][0].real * ( k * (1 << 15)));//multiply by k
	y[1][0].real= (y[1][0].real * ( k * (1 << 15)));//multiply by k

	y[0][0] = y[0][0] >> 15;
	y[1][0] = y[1][0] >> 15;


	if (quad == 4)
	{
		temp = y[0][0];
		y[0][0] = y[1][0];
		y[1][0].real = -temp.real;
	}//swap sin and cosine. sin is negative
	else if (quad == 3)
	{
		y[0][0].real = -y[0][0].real;
		y[1][0].real = -y[1][0].real;
	}//both are negative
	else if (quad == 2)
	{
		temp = y[0][0];
		y[0][0].real = -y[1][0].real;
		y[1][0] = temp;
	}//swap sin and cosine. cosine is negative
//	cout << "Cosine: " << y[0][0].real/32768.0 << endl;
//	cout << "Sine: " << y[1][0].real/32768.0 << endl;
	*sin = y[1][0];
	*cos = y[0][0];
	return;

}

cordic::~cordic() {

}

