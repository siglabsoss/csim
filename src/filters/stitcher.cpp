/*
 * stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include <filters/stitcher.hpp>

using namespace std;

Stitcher::Stitcher(int* waveNums, int* sampless, int numsSections)
{
    val = new int[numsSections]; //Which wave is at which time

    for (int i = 0; i < numsSections; i++) {
        val[i] = waveNums[i]; //copies values
    }

    samples = new int[numsSections]; //What percent of time that wave is used
    sample_total = 0;
    int percent_total = 0;
    for (int i = 0; i < numsSections; i++) {
        sample_total += sampless[i];
        samples[i] = sampless[i];
    }

    //assert(percent_total==100);

    numSections = numsSections; //Number of percents/vals

}


void Stitcher::doStuff(int val2, int i, vector<FixedComplex<32> > data)
{

	if (val2 == 0) {
		if (currentTheta > 205887) //2pi * 32768
			{
				endTheta -= 205887;
				currentTheta -= 205887;
			} //Shift currentTheta and endTheta down by 2pi if currentTheta is above 2pi

		for (int j = 0; j < samples[i]; j++) {
			 currentTheta = currentTheta + delta;
			output.push_back(data[counter++]);
		} //prints out actual data
	} //For wave data

	else {
		for (int j = 0; j < samples[i]; j++) {
			if (currentTheta > 205887) //2pi * 32768
			{
				endTheta -= 205887;
				currentTheta -= 205887;
			} //Shift currentTheta and endTheta down by 2pi if currentTheta is above 2pi

			c.calculate(currentTheta, a1, b1, &sinup, &sindown, &cosup,&cosdown); //Use cordic to calculate clock up
			FixedComplex<32> result;
			if (val2 == 1) {
				result.real = cosup;
				result.imag = sinup;
			}//Creates FixedComplex value to add to vector. CLOCKUP
			else {
				result.real = cosdown;
				result.imag = sindown;
			}//Creates FixedComplex value to add to vector. CLOCKDOWN
			output.push_back(result); //Adds result to vector
			currentTheta = currentTheta + delta;
		}
	}//For clockup or clock down
}


vector<FixedComplex<32> > Stitcher::stitch(int numSamples, int sampleRate,
        int frequency, vector<FixedComplex<32> > data)
{

	startingTheta = 0;
	counter = 0;
	currentTheta = 0;
    sc_int<32> totalTime = 0; //numSamples/sampleRate; // time in milliseconds?
    bool scaled = false;
    if (numSamples > sampleRate) {
        totalTime = numSamples / sampleRate;
    } else {
        totalTime = numSamples * 32768 / (sampleRate);
        scaled = true;
    }

    a1.real = 1;
    a1.imag = 0;
    b1.real = 0;
    b1.imag = 1;

    for (int i = 0; i < numSections; i++) {

        t = (totalTime * samples[i]) / sample_total; //total time of wave
        theta = 2 * 102943 * t * frequency; // theta must be between 0 and 2pi Equivalent to 2pi *  / t  pi * 32768 = 102943
        if (scaled) {
            theta = theta / 32768;
        }

        delta = 2 * 102943 * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second  pi * 32768 = 102943
        endTheta = startingTheta + theta; //new ending point is starting point + how many radians to use current wave
        doStuff(val[i], i ,  data);// Performs calulcations

        startingTheta = currentTheta; //Sets starting point for next wave. Add whatever is left over
    }

    return output;

}
