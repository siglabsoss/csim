/*
 * cordic.h
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#ifndef CORDIC_H_
#define CORDIC_H_

#include <iostream>
#include <types/fixedcomplex.hpp>

typedef sc_fixed<32, 5, SC_RND, SC_SAT> cordic_theta_t;
typedef FixedComplex2<32, 5> cordic_complex_t;
typedef sc_fixed<32, 5, SC_RND, SC_SAT> cordic_scalar_t;

//These typedefs can be used to enable floating point
//typedef float cordic_theta_t;
//typedef std::complex<float> cordic_complex_t;
//typedef float cordic_scalar_t;


class cordic
{
public:
    cordic();
    cordic_scalar_t sin(const cordic_theta_t &theta);
    cordic_scalar_t cos(const cordic_theta_t &theta);
    void calculate(cordic_theta_t theta, cordic_complex_t &sine, cordic_complex_t &cosine);

private: //methods
    /**
     * Rotates a given angle to the fist quadrant
     * @param theta (radians) to rotate, modified to resulting theta in first quadrant
     * @return the quadrant in which the original angle laid in
     */
    int rotateToFirstQuadrant(cordic_theta_t &theta);

    /**
     * Clips the complex values between [-1.0, 1.0]
     */
    void clipResults(cordic_complex_t y[2]);

    void rotateToQuadrant(cordic_complex_t y[2], int quadrant); //swaps cosine and sign or changes sign if necessary

private: //members
    std::vector <cordic_scalar_t > m_vals;

private: //constants
    static constexpr size_t NUM_HARDWIRED_VALUES = 40;
    static constexpr double k = .607252935; // k is a scaling factor that's a function of the number of hardwired values, this particular value was found emperically
};

#endif /* CORDIC_H_ */
