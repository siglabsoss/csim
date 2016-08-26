/*
 * stitcher.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include <filters/stitcher.hpp>

using namespace std;

Stitcher::Stitcher(int* waveNums, std::vector<int> sampless)
{
    m_val = new int[sampless.size()]; //Which wave is at which time

    for (int i = 0; i < sampless.size(); i++) {
    	m_val[i] = waveNums[i]; //copies values
    }

    m_samples = new int[sampless.size()]; //What percent of time that wave is used
    m_sample_total = 0;
    int percent_total = 0;
    for (int i = 0; i < sampless.size(); i++) {
    	m_sample_total += sampless[i];
    	m_samples[i] = sampless[i];
    }

    //assert(percent_total==100);

    m_numSections = sampless.size(); //Number of percents/vals

}

void Stitcher::shiftTheta()
{
	if (m_currentTheta > 205887) //2pi * 32768
	{
		m_endTheta -= 205887;
		m_currentTheta -= 205887;
	} //Shift m_currentTheta and m_endTheta down by 2pi if m_currentTheta is above 2pi

}

void Stitcher::doStuff(int val2, int i, vector<FixedComplex32 > data)
{

	if (val2 == 0) {
		shiftTheta();
		for (int j = 0; j < m_samples[i]; j++) {
			m_output.push_back(data[m_counter++]);
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
			if (val2 == 1) {
				result.real(sine.imag().range().to_int64()/134217728.0);//cosup;
				result.imag(sine.real().range().to_int64()/134217728.0);//sinup;
			}//Creates FixedComplex value to add to vector. CLOCKUP
			else {
				result.real(cosine.real().range().to_int64()/(134217728.0));//cosdown;
				result.imag(cosine.imag().range().to_int64()/(134217728.0));//sindown;
			}//Creates FixedComplex value to add to vector. CLOCKDOWN

			m_output.push_back(result); //Adds result to vector
			m_currentTheta = m_currentTheta + m_delta;
		}
	}//For clockup or clock down
}


vector<FixedComplex32 > Stitcher::stitch(int numSamples, int sampleRate,
        int frequency, vector<FixedComplex32 > data)
{
	reset();//Resets necessary values

    bool scaled = false;
    if (numSamples > sampleRate) {
        totalTime = numSamples / sampleRate;
    } else {
        totalTime = numSamples * 32768 / (sampleRate);
        scaled = true;
    }

    for (int i = 0; i < m_numSections; i++) {

        m_t = (totalTime * m_samples[i]) / m_sample_total; //total time of wave
        m_theta = 2 * 102943 * m_t * frequency; // theta must be between 0 and 2pi Equivalent to 2pi *  / t  pi * 32768 = 102943
        if (scaled) {
            m_theta = m_theta / 32768;
        }

        m_delta = 2 * 102943 * frequency / sampleRate; //increment of angles between samples. 2pi * number of waves per second / number of samples per second  pi * 32768 = 102943
        m_endTheta = m_currentTheta + m_theta; //new ending point is starting point + how many radians to use current wave
        doStuff(m_val[i], i ,  data);// Performs calculations
    }
    return m_output;

}

void Stitcher::reset()
{
	m_counter = 0; //Resets m_counter for which part of the data the stitcher is on
	m_currentTheta = 0; //Resets
	totalTime = 0; //numSamples/sampleRate; // time in milliseconds?
	m_output.clear();

}
