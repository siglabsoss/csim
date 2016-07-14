/*
 * Stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include "Stitcher.h"

using namespace std;

Stitcher::Stitcher(int* waveNums, int* percents, int nums, vector<float> datas)
{
	val = new int[nums];

	for (int i = 0; i < nums; i++)
		val[i] = waveNums[i];

	percent = new int[nums];
	for (int i = 0; i < nums; i++)
		percent[i] = percents[i];

	num = nums;
	data = datas;

}

vector<float> Stitcher::stitch(int numSamples, int sampleRate, int frequency)
{
	float totalTime = numSamples/sampleRate;

	cordic c;
	FixedComplex<16> a1(1,0);
	FixedComplex<16> b1(0,1);
	sc_int<32> cosdown;
	sc_int<32> cosup;
	sc_int<32> sinup;
	sc_int<32> sindown;
	int count = 0;
	float currentAngle = 0;
	for (int i = 0; i < num; i++)
	{
		count = 0;

		float a = totalTime * percent[i] / 100.0; //total time of wave
		float b = a;
		float t = 1.0/frequency; //Period of wave

		while (b > t)
		{
			b = b - t;
			count++;
		}//While b is greater than 1 wave time

		float theta = 2 * 3.14159 * b * frequency; // theta must be between 0 and 2pi Equivalent to 2pi * b / t
		float delta = 2 * 3.14159 * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second

		if (val[i] == 0)
		{
			while (count > 0)
			{
				for (float l = 0; l < 2 * 3.14159; l = l + delta)
				{
					int randomNum = data[rand() % 10];
					output.push_back(randomNum/10.0);
				}//print out 1 entire wave
				count--;
			}//print out count entire waves

			for (float m = 0; m < theta; m = m + delta)
			{
				int randomNum = rand() % 19 + (-9);
				output.push_back(randomNum/10.0);
			}//prints out remainder of wave

		}

		if (val[i] == 1)
		{
			while (count > 0)
			{
				for (float l = 0; l < 2 * 3.14159; l = l + delta)
				{
					sc_int<20> k = l * 32768;

					c.calculate(k,a1,b1,&sinup,&sindown, &cosup, &cosdown);
					output.push_back(sinup/32768.0);

				}//print out 1 entire wave
				count--;
			}//print out count entire waves

			for (float m = 0; m < theta; m = m + delta)
			{
				sc_int<20> k = m * 32768;
				c.calculate(k,a1,b1,&sinup,&sindown, &cosup, &cosdown);
				output.push_back(sinup/32768.0);
			}//prints out remainder of wave


		}

		if (val[i] == 2)
		{
			while (count > 0)
			{
				for (float l = 0; l < 2 * 3.14159; l = l + delta)
				{
					sc_int<20> k = l * 32768;
					c.calculate(k,a1,b1,&sinup,&sindown, &cosup, &cosdown);
					output.push_back(sindown/32768.0);
				}//print out 1 entire wave
				count--;
			}//print out count entire waves

			for (float m = 0; m < theta; m = m + delta)
			{
				sc_int<20> k = m * 32768;
				c.calculate(k,a1,b1,&sinup,&sindown, &cosup, &cosdown);
				output.push_back(sindown/32768.0);
			}//prints out remainder of wave
		}
	}

	return output;

}


Stitcher::~Stitcher() {
	// TODO Auto-generated destructor stub
}


