/*
 * Stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include "Stitcher.h"

using namespace std;

Stitcher::Stitcher() {
	totalTime = 100;

	// TODO Auto-generated constructor stub

}
void Stitcher::stitch()
{
	 string outFile = "data/output/stitching.csv";
	 ofstream out(outFile.c_str());

	for (int time = 0; time < totalTime; time++)
	{

		cordic c;
		FixedComplex<16> a(1,0);
		FixedComplex<16> b(0,0);
		FixedComplex<32> sin;
		FixedComplex<32> cos;
		if (timer[time] == 0)
		{
			sc_int<20> j = time/100.00 * 32768;
			c.calculate(j,a,b,&sin,&cos);
			//cout << "Cosine: " << cos.real/32768.0 << endl;
			//cout << "Sine: " << sin.real/32768.0 << endl;
			//out << cos.real/32768.0<<",";
			out << sin.real/32768.0 << "," << endl;
		}

		if (timer[time] == 1)
		{
			int randomNum = rand() % 19 + (-9);
			out << randomNum/10.0 << "," << endl;
		}

		if (timer[time] == 2)
		{
			sc_int<20> j = time/100.00 * 32768;
			c.calculate(j,a,b,&sin,&cos);
			//cout << "Cosine: " << cos.real/32768.0 << endl;
			//cout << "Sine: " << sin.real/32768.0 << endl;
			//out << cos.real/32768.0<<",";
			out << -sin.real/32768.0 << "," <<endl;
		}
		cout << time << endl;
	}

	return;
}

void Stitcher::format(int* wave, int* percent)
{
	int j = 0;
	int temp;
	int currentTime = 0;
	for ( int i = 0; i < 3; i++)
	{
		 temp = percent[i] * totalTime /100;
		for (j = 0; j < (temp); j++)
		{
			timer[currentTime] = wave[i];
			currentTime++;
		}

	}

}
Stitcher::~Stitcher() {
	// TODO Auto-generated destructor stub
}


