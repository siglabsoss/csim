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
	Stitcher(int* waveNums, int* percents, int nums);
	void stitch(int numSamples, int sampleRate, int frequency);

	virtual ~Stitcher();

	int* val;
	int num;
	int* percent;
	vector<float> data;




};


#endif /* STITCHER_H_ */
