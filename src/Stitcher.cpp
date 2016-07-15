/*
 * Stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include "Stitcher.h"

using namespace std;

Stitcher::Stitcher(int* waveNums, int* percents, int nums, vector<FixedComplex<32> > datas)
{
	val = new int[nums];//Which wave is at which time

	for (int i = 0; i < nums; i++)
		val[i] = waveNums[i];//copies values

	percent = new int[nums];//What percent of time that wave is used
	for (int i = 0; i < nums; i++)
		percent[i] = percents[i];

	num = nums;//Number of percents/vals
	data = datas;//input data;
}

vector<FixedComplex<32> > Stitcher::stitch(int numSamples, int sampleRate, int frequency)
{
	float totalTime = numSamples/sampleRate;

	cordic c;
	FixedComplex<16> a1(1,0);
	FixedComplex<16> b1(0,1);
	sc_int<32> cosdown;
	sc_int<32> cosup;
	sc_int<32> sinup;
	sc_int<32> sindown;
	sc_int<20> k;
	float thetaa;
	//int count = 0;
	float currentTheta = 0;//Keeps track of current position
	for (int i = 0; i < num; i++)
	{
//		count = 0;
		float a = totalTime * percent[i] / 100.0; //total time of wave
		float b = a;
		float t = 1.0/frequency; //Period of wave
//
//		while (b > t)
//		{
//			b = b - t;
//			count++;
//		}//While b is greater than 1 wave time

	//	float theta = 2 * 3.14159 * b * frequency; // theta must be between 0 and 2pi Equivalent to 2pi * b / t
		thetaa = 2 * 3.14159 * a * frequency; // theta must be between 0 and 2pi Equivalent to 2pi * b / t
		float delta = 2 * 3.14159 * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second

		if (val[i] == 0)
			for (float m = currentTheta; m < currentTheta + thetaa; m = m + delta)
			{
				if (m > 2 * 3.14159)
				{
					currentTheta-=2*3.14159;
					m-=2*3.14159;
				}//Shift theta down by 2pi if above it

				output.push_back(data[rand() % 32768]);
			}//prints out wave

		if (val[i] == 1)
			for (float m = currentTheta; m < thetaa + currentTheta; m = m + delta)
			{
				if (m > 2 * 3.14159)
				{
					currentTheta-=2*3.14159;
					m-=2*3.14159;
				}//Shift theta down by 2pi if above it

				k = m * 32768;
				c.calculate(k,a1,b1,&sinup,&sindown, &cosup, &cosdown);
				FixedComplex<32> clockup(cosup, sinup);
				output.push_back(clockup);
			}//prints out remainder of wave

		if (val[i] == 2)
			for (float m = currentTheta; m < thetaa + currentTheta; m = m + delta)
			{
				if (m > 2 * 3.14159)
				{
					currentTheta-=2*3.14159;
					m-=2*3.14159;
				}//Shift theta down by 2pi if above it

				k = m * 32768;
				c.calculate(k,a1,b1,&sinup,&sindown, &cosup, &cosdown);
				FixedComplex<32> clockdown(cosdown, sindown);
				output.push_back(clockdown);
			}//prints out remainder of wave

		currentTheta += thetaa;
	}


	return output;

}



