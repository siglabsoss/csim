/*
 * stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include <filters/stitcher.hpp>

Stitcher::Stitcher(std::vector<int> waveNums, std::vector<int> samples)
{
	m_val = waveNums; //copies values
	m_samples = samples;
	m_sample_total = 0;
	for (int i = 0; i < samples.size(); i++) {
    	m_sample_total += samples[i];//Gets total number of samples to be stitched
    }
    m_numSections = samples.size(); //Number of vals
}

void Stitcher::reset()
{
	m_inputData.clear();
	m_counter = 0; //Resets m_counter for which part of the data the stitcher is on
	m_currentTheta = 0; //Resets
	totalTime = 0; //numSamples/sampleRate; // time in milliseconds?
	m_output.clear();
}

void Stitcher::shiftTheta()
{
	if (m_currentTheta > 205887) //2pi * 32768
	{
		m_endTheta -= 205887;
		m_currentTheta -= 205887;
	} //Shift m_currentTheta and m_endTheta down by 2pi if m_currentTheta is above 2pi
}

std::vector<FixedComplex32 > Stitcher::stitch(int numSamples, int sampleRate,
        int frequency, std::vector<FixedComplex32 > data)
{
	reset();//Resets necessary values
	m_inputData = data;
	totalTime = numSamples / sampleRate;

    for (int i = 0; i < m_numSections; i++) {
        m_t = (totalTime * m_samples[i]) / m_sample_total; //total time of wave
        m_theta = 2 * 102943 * m_t * frequency; // theta must be between 0 and 2pi Equivalent to 2pi *  / t  pi * 32768 = 102943
        m_delta = 2 * 102943 * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second  pi * 32768 = 102943
        m_endTheta = m_currentTheta + m_theta; //new ending point is starting point + how many radians to use current wave
        doStuff(i);// Performs calculations
    }

    return m_output;
}

void Stitcher::doStuff(int i)
{
	int waveNum = m_val[i];
	if (waves[waveNum] == DATA) {
		shiftTheta();
		for (int j = 0; j < m_samples[i]; j++) {
			m_output.push_back(m_inputData[m_counter++]);
			m_currentTheta = m_currentTheta + m_delta;
		} //prints out actual data
	} //For wave data

	else {
		for (int j = 0; j < m_samples[i]; j++) {
			shiftTheta();
			cordic_theta_t theta2(m_currentTheta/32768.0);
			cordic_complex_t sine;
			cordic_complex_t cosine;
			c.calculate(theta2, sine, cosine); //Use cordic to calculate clock up
			FixedComplex32 result;

			if (waves[waveNum] == CLOCKUP) {
				result.real(sine.imag().range().to_int64()/134217728.0);//cosup;
				result.imag(sine.real().range().to_int64()/134217728.0);//sinup;
			}//Creates FixedComplex value to add to std::vector. CLOCKUP
			else if (waves[waveNum] == CLOCKDOWN) {
				result.real(cosine.real().range().to_int64()/(134217728.0));//cosdown;
				result.imag(cosine.imag().range().to_int64()/(134217728.0));//sindown;
			}//Creates FixedComplex value to add to std::vector. CLOCKDOWN

			else {
				std::cout << "Unrecognized wave number: " << waves[waveNum] << std::endl;
				throw "Unrecognized wave number";
			}// !(0|1|2)

			m_output.push_back(result); //Adds result to std::vector
			m_currentTheta = m_currentTheta + m_delta;
		}
	}//For clockup or clockdown
}


