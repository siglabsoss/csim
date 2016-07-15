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
	Stitcher(int* waveNums, int* percents, int nums, vector<FixedComplex<32> > datas);//Takes in array of corresponding wave number, percent array,number of values in each of those arrays, and wave data
	vector<FixedComplex<32> > stitch(int numSamples, int sampleRate, int frequency);//stitches data based on inputs given. Returns as a vector of fixedcomplex

	int* val; //which waves to use at which section
	int num; //number of sections of output
	int* percent;//array that shows what percent of the final output will be that wave
	vector<FixedComplex<32> > data; //data in
	vector<FixedComplex<32> > output; //data outs



};


#endif /* STITCHER_H_ */
