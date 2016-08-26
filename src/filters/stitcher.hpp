/*
 * stitcher.h
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#ifndef STITCHER_H_
#define STITCHER_H_

#include <mathlib/cordic.hpp>

#include <fstream>
#include <iostream>
#include <stdlib.h> //rand()
#include <iomanip>
#include <vector>
class Stitcher
{
public:
	cordic c;
	FixedPoint<32,1> cosdown;
	FixedPoint<32,1> cosup;
	FixedPoint<32,1> sinup;
	FixedPoint<32,1> sindown;
	FixedPoint<32,32> theta; //How many radians to run a certain wave
	FixedPoint<32,32> t; //Total time that wave is used
	FixedPoint<32,32> delta; //Total angle that wave is used in radians
	FixedPoint<32,32> endTheta; // Angle to end on
	int counter = 0; //Which sample of actual data you are on
	sc_int<32> currentTheta; //Current angle in radians
	void shiftTheta();
    Stitcher(int* waveNums, int* samples, int numSections); //Takes in array of corresponding wave number,number of samples for each wave number, and number of sections
    std::vector<FixedComplex32 > stitch(int numSamples, int sampleRate,
            int frequency, std::vector<FixedComplex32 > data); //stitches data based on inputs given. Returns as a vector of fixedcomplex

    int* val; //which waves to use at which section
    int numSections; //number of sections of output
    int* samples; //array that shows how many samples of the final output will be that wave
    int sample_total;
    std::vector<FixedComplex32 > output; //data outs
    void doStuff(int val, int i, std::vector<FixedComplex32 > data);
};

#endif /* STITCHER_H_ */
