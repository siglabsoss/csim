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
			vals[i] = .785398  * (1 << 15);
		else if (i == 1)
			vals[i] = .463648 * (1 << 15);
		else if (i == 2)
			vals[i] = .244979 * (1 << 15);
		else if (i == 3)
			vals[i] = .124355 * (1 << 15);
		else if (i == 4 )
			vals[i] = .062419 * (1 << 15);
		else
			vals[i] = 1 << (15 - i);//((float)1/(1 << (i))) * (1 << 15);

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
	for (int i = 0; i < 16; i++)
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

	for (int i = 0; i < 16; i++)
	{
		z[0][0].real = 1 << 15; //32768. Upper left

		z[0][1].real = -(1<< (15 - i));
		z[1][0].real = -z[0][1].real; // Lower left
		z[1][1].real = 1 << 15; // 1. Lower right
		temp = y[0][0]; //temporary storage for later calculation

		c1 = (((y[0][0])));//(((z[0][0] * y[0][0]) >> 15)); z[0][0].real = 1 << 15
		c2 = (y[1][0] >> i) ; //(((z[0][1]) * (y[1][0].real))  >> 15); z[0][1].real = -(1<< (15 - i));
		c2.real = -c2.real;//-(1<< (15 - i));
		if (sign[i] == -1)
			c2.real = -c2.real;//multiply by rotation direction
		y[0][0] = c1 + c2;//Upper value

		c1 = ((y[1][0]));//((z[1][1] * y[1][0]) >> 15) z[1][1] = 1 << 15
		c2 = (temp) >> i;//	c2 = ((z[1][0] * temp) >> 15);
		if (sign[i] == -1)
			c2.real = -c2.real;//multiply by rotation direction
		y[1][0] = c2 + c1;//Lower value

	}

	y[0][0].real = (y[0][0].real * ( k * (1 << 15)));//multiply by k
	y[1][0].real= (y[1][0].real * ( k * (1 << 15)));//multiply by k

	y[0][0] = y[0][0] >> 15;
	y[1][0] = y[1][0] >> 15;

	signs();

//	cout << "Cosine: " << y[0][0].real/32768.0 << endl;
//	cout << "Sine: " << y[1][0].real/32768.0 << endl;
	*sin = y[1][0];
	*cos = y[0][0];
	return;

}

void cordic::signs()
{
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
}//swaps cosine and sign or changes sign if necessary.


cordic::~cordic() {

}

