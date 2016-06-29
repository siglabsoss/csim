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


void fixedfir::fir(int length, FixedComplex<16>* input, FixedComplex<16>* output)
{
	FixedComplex<16> bench[this->n];
	FixedComplex<32> sum;

	sum.real = 1<<14;
	sum.imag = 1<<14;

	for (int i = 0; i < length; i++)
	{
		bench[0] = input[i];//New data on bench

		sum.real = 1 << 14;
		sum.imag = 1 << 14;
		for ( int j = 0; (j < this->n ); j++)
		{
			sum = sum + (bench[j].to_32() * taps[j].to_32());
		}//Accumulate

		for (int j = this->n - 1 ; j > 0; j--)
		{
			bench[j] = bench[j-1];
		}//Moves all data down by 1 space

        if ( sum.real > 0x3fffffff ) {
            sum.real = 0x3fffffff;
        } else if ( sum.real < -0x40000000 ) {
            sum.real = -0x40000000;
        }
        if ( sum.imag > 0x3fffffff ) {
                    sum.imag = 0x3fffffff;
                } else if ( sum.imag < -0x40000000 ) {
                    sum.imag = -0x40000000;
                }

		sum = sum >> 15;
		output[i] = sum.to_16();

	}

}

fixedfir::~fixedfir() {

}

