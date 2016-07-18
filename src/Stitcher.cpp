/*
 * Stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include "Stitcher.h"

using namespace std;

Stitcher::Stitcher(int* waveNums, int* sampless, int numsSections)
{
	val = new int[numsSections];//Which wave is at which time

	for (int i = 0; i < numsSections; i++)
	{
		val[i] = waveNums[i];//copies values
	}

	samples = new int[numsSections];//What percent of time that wave is used
	sample_total = 0;
	int percent_total = 0;
	for (int i = 0; i < numsSections; i++)
	{
		sample_total += sampless[i];
		samples[i] = sampless[i];
	}

	//assert(percent_total==100);

	numSections = numsSections;//Number of percents/vals

}

vector<FixedComplex<32> > Stitcher::stitch(int numSamples, int sampleRate, int frequency, vector<FixedComplex<32> > data)
{
	sc_int<32> totalTime = 400; //numSamples/sampleRate; // time in milliseconds?

	cordic c;
	FixedComplex<16> a1(1,0);//coefficient for cordic
	FixedComplex<16> b1(0,1);//coefficient for cordic
	sc_int<32> cosdown;
	sc_int<32> cosup;
	sc_int<32> sinup;
	sc_int<32> sindown;
	sc_int<32> theta;//How many radians to run a certain wave
	sc_int<32> startingTheta = 0;//Keeps track of original position
	sc_int<32> t;//Total time that wave is used
	sc_int<32> delta;//Total angle that wave is used in radians
	sc_int<32> endTheta; // Angle to end on
	int counter = 0; //Which sample of actual data you are on
	sc_int<32> currentTheta = 0;//Current angle in radians
	for (int i = 0; i < numSections; i++)
	{

		t = (totalTime * samples[i]) / sample_total; //total time of wave
		theta = 2 *  102943 * t * frequency; // theta must be between 0 and 2pi Equivalent to 2pi *  / t  pi * 32768 = 102943
		delta = 2 *  102943 * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second  pi * 32768 = 102943
		endTheta = startingTheta + theta;
		if (val[i] == 0)
		{
			for (int j = 0; j < samples[i]; j++)
			{
				if (currentTheta > 205887)//2pi * 32768
				{
					endTheta -= 205887;
					currentTheta-= 205887;
				}//Shift currentTheta and endTheta down by 2pi if currentTheta is above 2pi
				currentTheta = currentTheta + delta;
				output.push_back(data[counter++]);
			}//prints out actual data
		}

		if (val[i] == 1)
		{
			for (int j = 0; j < samples[i]; j++)
			{
				if (currentTheta > 205887)//2pi * 32768
				{
					endTheta -= 205887;
					currentTheta -= 205887;
				}//Shift currentTheta and endTheta down by 2pi if currentTheta is above 2pi

				c.calculate(currentTheta, a1, b1, &sinup, &sindown, &cosup, &cosdown);//Use cordic to calculate clock up
				FixedComplex<32> clockup(cosup, sinup);//Creates FixedComplex value to add to vector
				output.push_back(clockup);//Adds result to vector
				currentTheta= currentTheta + delta;
			}//Adds clockup wave to output
		}

		if (val[i] == 2)
		{
			for (int j = 0; j < samples[i]; j++)
			{
				if (currentTheta > 205887)//2pi * 32768
				{
					endTheta -= 205887;
					currentTheta -= 205887;
				}//Shift currentTheta and endTheta down by 2pi if currentTheta is above 2pi

				c.calculate(currentTheta,a1,b1,&sinup,&sindown, &cosup, &cosdown);//Use cordic to calculate clockdown
				FixedComplex<32> clockdown(cosdown, sindown);//Creates FixedComplex value to add to vector
				output.push_back(clockdown);//Adds result to vector
				currentTheta = currentTheta + delta;
			}//Adds clockdown wave to output
		}

		startingTheta = currentTheta;//Sets starting point for next wave. Add whatever is left over
	}

	return output;

}




