/*
 * fixedfir.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#include <fixedfir.h>

#define MAX_INPUT_LEN   80
// maximum length of filter than can be handled
#define MAX_FLT_LEN     63
// buffer to hold all of the input samples
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)

// array to hold input samples
FixedComplex<16> insamp[ BUFFER_LEN ];



fixedfir::fixedfir(int N, FixedComplex<16>* val) {

	this->n = N;
	this->vals = new FixedComplex<16>[N];

	for (int i = 0; i < N; i++)
		this->vals[i] = val[i];

	// TODO Auto-generated constructor stub

}

void fixedfir::firFixedInit()
{
    memset( insamp, 0, sizeof( insamp ) );
}

void fixedfir::fir(FixedComplex<16> *coeffs, int filterLength )
{
    FixedComplex<32> acc;     // accumulator for MACs
    FixedComplex<16> *coeffp; // pointer to coefficients
    FixedComplex<16> *inputp; // pointer to input samples
    int n;
    int k;

    // put the new samples at the high end of the buffer
    memcpy( &insamp[filterLength - 1], this->vals,
            this->n * sizeof(FixedComplex<16>));

    // apply the filter to each input sample
    for ( n = 0; n < this->n; n++ ) {
        // calculate output n
        coeffp = coeffs;
        inputp = &insamp[filterLength - 1 + n];
        // load rounding constant
        acc.real = 1 << 14;
        // perform the multiply-accumulate
        for ( k = 0; k < filterLength; k++ ) {
            acc += ( FixedComplex<32>)(*coeffp++) * ( FixedComplex<32>)(*inputp--);
        }
        // saturate the result
        if ( acc.real > 0x3fffffff ) 
            acc.real= 0x3fffffff; //Only real
        else if ( acc.real < -0x40000000 ) 
            acc.real = -0x40000000; //Only real
        
        // convert from Q30 to Q15
        output[n] = ( FixedComplex<16>)(acc >> 15);
    }

    // shift input samples back in time for next time
    memmove( &insamp[0], &insamp[this->n],
            (filterLength - 1) * sizeof( FixedComplex<16>) );

}

fixedfir::~fixedfir() {
	// TODO Auto-generated destructor stub
}

