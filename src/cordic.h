/*
 * cordic.h
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#ifndef CORDIC_H_
#define CORDIC_H_

#include <iostream>
#include "fixedcomplex.h"
class cordic
{
public:

    cordic();
    void rotate(sc_int<32> theta); //Calculates which direction to rotate
    void calculate(sc_int<32> theta, FixedComplex<16> a, FixedComplex<16> b,
            sc_int<32>* sinup, sc_int<32>* sindown, sc_int<32>* cosup,
            sc_int<32>* cosdown); //Calculates sine and cosine
    void signs(); //swaps cosine and sign or changes sign if necessary
    sc_int<32> sin(sc_int<32> theta);
    virtual ~cordic();

    int vals[16]; //Holds rotation values
    int sign[16]; //Holds which direction to rotate

    int quad;
    double k; //Constant
    FixedComplex<32> y[2][1]; //right aray to multiply (2x1)
    FixedComplex<32> z[2][2]; //left matrixto multiply by(2x2)
    FixedComplex<32> temp; //temporary storage for later calculation
    FixedComplex<32> c1; //a-(1/2^n)b
    FixedComplex<32> c2; //a(1/2^n)+b

};

#endif /* CORDIC_H_ */
