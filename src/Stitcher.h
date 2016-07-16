/*
 * Stitcher.h
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#ifndef STITCHER_H_
#define STITCHER_H_

#include "cordic.h"
#include <fstream>
#include <iostream>
#include <stdlib.h> //rand()
#include <iomanip>
#include <vector>
class Stitcher {
public:
	Stitcher(int* waveNums, int* samples, int numSections);//Takes in array of corresponding wave number, percent array,number of values in each of those arrays, and wave data
	vector<FixedComplex<32> > stitch(int numSamples, int sampleRate, int frequency, vector<FixedComplex<32> > data);//stitches data based on inputs given. Returns as a vector of fixedcomplex

	int* val; //which waves to use at which section
	int numSections; //number of sections of output
	int* samples;//array that shows how many samples of the final output will be that wave
	int sample_total;
	vector<FixedComplex<32> > output; //data outs



};


#endif /* STITCHER_H_ */
