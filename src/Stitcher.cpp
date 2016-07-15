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
	sc_int<32> theta;
	sc_int<32> currentTheta = 0;//Keeps track of current position
	sc_int<32> a;
	sc_int<32> delta;

	for (int i = 0; i < num; i++)
	{

		a = totalTime * percent[i] / 100.0 * 32768; //total time of wave
		theta = 2 * 3.14159 * a * frequency; // theta must be between 0 and 2pi Equivalent to 2pi * b / t
		delta = 2 * 3.14159 * frequency / sampleRate * 32768; //increment of angles between samples. 2pi * number of waves per second / number of samples per second

		if (val[i] == 0)
			for (sc_int<32> m = currentTheta; m < currentTheta + theta; m = m + delta)
			{

				if (m > 205887)//2pi * 32768
				{
					currentTheta-=205887;
					m-=205887;
				}//Shift theta down by 2pi if above it

				output.push_back(data[rand() % 32768]);
			}//prints out wave

		if (val[i] == 1)
			for (sc_int<32> m = currentTheta; m < theta + currentTheta; m = m + delta)
			{
				if (m > 205887)//2pi * 32768
				{
					currentTheta-=205887;
					m-=205887;
				}//Shift theta down by 2pi if above it

				c.calculate(m,a1,b1,&sinup,&sindown, &cosup, &cosdown);
				FixedComplex<32> clockup(cosup, sinup);
				output.push_back(clockup);
			}//prints out remainder of wave


		if (val[i] == 2)
			for (sc_int<32> m = currentTheta; m < theta + currentTheta; m = m + delta)
			{
				if (m > 205887)//2pi * 32768
				{
					currentTheta-=205887;
					m-=205887;
				}//Shift theta down by 2pi if above it


				c.calculate(m,a1,b1,&sinup,&sindown, &cosup, &cosdown);
				FixedComplex<32> clockdown(cosdown, sindown);
				output.push_back(clockdown);
			}//prints out remainder of wave

		currentTheta += theta;
	}


	return output;

}



