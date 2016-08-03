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

typedef sc_ufixed<62, 31> cordic_theta_t;
typedef FixedComplex2<62, 31> cordic_complex_t;
typedef sc_fixed<62, 31> cordic_scalar_t;

//typedef double cordic_theta_t;
//typedef std::complex<double> cordic_complex_t;
//typedef double cordic_scalar_t;


class cordic
{
public:
    cordic();
    void rotate(cordic_theta_t theta); //Calculates which direction to rotate
    void calculate(cordic_theta_t theta, cordic_complex_t a, cordic_complex_t b,
            cordic_scalar_t* sinup, cordic_scalar_t* sindown, cordic_scalar_t* cosup,
            cordic_scalar_t* cosdown); //Calculates sine and cosine
    void signs(); //swaps cosine and sign or changes sign if necessary
    cordic_scalar_t sin(cordic_scalar_t theta);
    virtual ~cordic();

    std::vector <cordic_scalar_t > vals;
    std::vector <int> sign;
    int quad;

    cordic_complex_t y[2]; //right aray to multiply (2x1)
    cordic_complex_t temp; //temporary storage for later calculation
    cordic_complex_t c1; //a-(1/2^n)b
    cordic_complex_t c2; //a(1/2^n)+b

private:
    static constexpr size_t NUM_HARDWIRED_VALUES = 16;
    static constexpr double k = .607252935; //constant k (using k for 40 values despite actually using less in this implementation as it was found to provide better results)
};

#endif /* CORDIC_H_ */
