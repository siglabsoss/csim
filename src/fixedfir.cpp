/*
 * fixedfir.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#include <fixedfir.h>

#define FILTER_LEN  63
#define MAX_INPUT_LEN   80// maximum length of filter than can be handled
#define MAX_FLT_LEN     63// buffer to hold all of the input samples
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)

// array to hold input samples




fixedfir::fixedfir(int N, FixedComplex<16>* tap) {

	this->n = N;
	this->taps = new FixedComplex<16>[N];

	for (int i = 0; i < N; i++)
		this->taps[i] = tap[i];


}

void fixedfir::firFixedInit()
{
    
}

void fixedfir::fir(FixedComplex<16>* output)
{

    FixedComplex<32> acc;     // accumulator for MACs
    FixedComplex<16> *coeffp; // pointer to coefficients
    FixedComplex<16> *inputp; // pointer to input samples

    int k;
    FixedComplex<16> insamp[BUFFER_LEN];
    // put the new samples at the high end of the buffer
    memcpy( &insamp[FILTER_LEN - 1], this->taps,
            this->n * sizeof(FixedComplex<16>));

    // apply the filter to each input sample
    for (int n = 0; n < this->n; n++ ) {
        // calculate output n
        coeffp = this->taps;
        inputp = &insamp[FILTER_LEN - 1 + n];
        // load rounding constant
        acc.real = 1 << 14;
        
        // perform the multiply-accumulate
        for ( k = 0; k < FILTER_LEN; k++ ) {
            acc = acc + ( (*coeffp++).to_32() * (*inputp--).to_32());
        }
        // saturate the result
        if ( acc.real > 0x3fffffff ) 
            acc.real= 0x3fffffff; //Only real
        else if ( acc.real < -0x40000000 ) 
            acc.real = -0x40000000; //Only real
        
        // convert from Q30 to Q15
        output[n] = (acc >> 15).to_16();//Only Real?
    }

    // shift input samples back in time for next time
    memmove( &insamp[0], &insamp[this->n],
            (FILTER_LEN - 1) * sizeof( FixedComplex<16>) );


}

fixedfir::~fixedfir() {
	// TODO Auto-generated destructor stub
}

