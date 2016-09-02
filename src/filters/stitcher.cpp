/*
 * stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include <filters/stitcher.hpp>

Stitcher::Stitcher(std::vector<int> waveNums, std::vector<int> samples)
{
	reset();
	m_val = waveNums; //copies values of the order of the waves
	m_samples = samples; //How many sample to play for each segment of wave
	m_sample_total = 0; //Total number of samples
	for (unsigned int i = 0; i < samples.size(); i++) {
    	m_sample_total += samples[i]; //Gets total number of samples to be stitched
    }
    m_numSections = samples.size(); //Number of sections to be stitched
}

void Stitcher::reset()
{
	m_inputData.clear(); //Clears input vector
	m_counter = 0; //Resets m_counter for which part of the data the stitcher is on
	m_currentTheta = 0; //Resets
	totalTime = 0; //numSamples/sampleRate; // time in milliseconds?
	m_output.clear(); //Clears output vector
}

void Stitcher::shiftTheta()
{
	while (m_currentTheta > 2 * M_PI) //2pi * 32768
	{
		m_endTheta -= 2 * M_PI;
		m_currentTheta -= 2 * M_PI;
	} //Shift m_currentTheta and m_endTheta down by 2pi if m_currentTheta is above 2pi
}

std::vector<FixedComplex32 > Stitcher::stitch(int numSamples, int sampleRate,
        int frequency, std::vector<FixedComplex32 > data)
{
	reset();//Resets necessary values
	m_inputData = data; //Sets new input data
	totalTime = numSamples / sampleRate;

    for (int i = 0; i < m_numSections; i++) {
        m_t = (totalTime * m_samples[i]) / m_sample_total; //total time of wave
        m_theta = 2 * M_PI * m_t * frequency; //How many radians to use current wave
        m_delta = 2 * M_PI * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second
        m_endTheta = m_currentTheta + m_theta; //new ending point is (starting point + how many radians to use current wave)
        outputWave(i);// Performs calculations
    }

    return m_output;
}

void Stitcher::outputWave(int i)
{
	int waveNum = m_val[i];
	if (waves[waveNum] == DATA) {
		for (int j = 0; j < m_samples[i]; j++) {
			shiftTheta();
			m_output.push_back(m_inputData[m_counter++]);
			m_currentTheta = m_currentTheta + m_delta;
		} //prints out actual data
	} //For wave data

	else {
		for (int j = 0; j < m_samples[i]; j++) {
			shiftTheta();
			cordic_theta_t theta2(m_currentTheta);
			cordic_complex_t sine; //For results of cordic to be stored in
			cordic_complex_t cosine; //For results of cordic to be stored in
			c.calculate(theta2, sine, cosine); //Use cordic to calculate clock up
			FixedComplex32 result;

			if (waves[waveNum] == CLOCKUP) {
				result.real(sine.imag());//cosup;
				result.imag(sine.real());//sinup;
			}//Creates FixedComplex value to add to std::vector. CLOCKUP
			else if (waves[waveNum] == CLOCKDOWN) {
				result.real(cosine.real());//cosdown;
				result.imag(cosine.imag());//sindown;
			}//Creates FixedComplex value to add to std::vector. CLOCKDOWN

			else {
				std::cout << "Unrecognized wave number: " << waves[waveNum] << std::endl;
				throw "Unrecognized wave number";
			}// !(0|1|2)

			m_output.push_back(result); //Adds result to std::vector
			m_currentTheta = m_currentTheta + m_delta; //Move forwards by delta radians
		}
	}//For clockup or clockdown
}


