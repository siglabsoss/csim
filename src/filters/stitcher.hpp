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

enum waveType
{
    DATA, CLOCKUP, CLOCKDOWN
};

class Stitcher
{
public:
	waveType waves[3] = {DATA, CLOCKUP, CLOCKDOWN};
	cordic c;
	FixedPoint<32,32> m_theta; //How many radians to run a certain wave
	FixedPoint<32,32> m_t; //Total time that wave is used
	FixedPoint<32,32> m_delta; //Total angle that wave is used in radians
	FixedPoint<32,32> m_endTheta; // Angle to end on
	int m_counter = 0; //Which sample of actual data you are on
	sc_int<32> m_currentTheta; //Current angle in radians
	void shiftTheta();
    Stitcher(std::vector<int> waveNums, std::vector<int> samples); //Takes in array of corresponding wave number,number of samples for each wave number, and number of sections
    std::vector<FixedComplex32 > stitch(int numSamples, int sampleRate,
            int frequency, std::vector<FixedComplex32 > data); //stitches data based on inputs given. Returns as a vector of fixedcomplex

    FixedPoint<32,32> totalTime;
    std::vector<int> m_val; //which waves to use at which section
    int m_numSections; //number of sections of output
    std::vector<int> m_samples; //array that shows how many samples of the final output will be that wave
    int m_sample_total;
    std::vector<FixedComplex32 > m_output; //data outs
    void doStuff(int val, int i, std::vector<FixedComplex32 > data);
    void reset();
};



#endif /* STITCHER_H_ */
