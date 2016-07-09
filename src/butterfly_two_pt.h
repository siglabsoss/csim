/*
 * butterfly_two_pt.h
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "fixedcomplex.h"

#ifndef BUTTERFLY_TWO_PT_H_
#define BUTTERFLY_TWO_PT_H_




SC_MODULE (butterfly_two_pt)
{
	sc_in <FixedComplex<32> > x,y;
	sc_out <FixedComplex<32> > result[2];

	void butterfly_fft ();

	SC_CTOR (butterfly_two_pt)
	{
		SC_METHOD (butterfly_fft);
		sensitive(x,y);
	}
};


#endif /* BUTTERFLY_TWO_PT_H_ */
