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

class Stitcher {
public:
	Stitcher();
	void stitch();
	void format(int* wave, int* percent);
	virtual ~Stitcher();
	int totalTime;
	int timer[100];

};


#endif /* STITCHER_H_ */
