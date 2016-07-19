/*
 * butterfly_two_pt.h
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "complex_alu.h"

#ifndef BUTTERFLY_TWO_PT_H_
#define BUTTERFLY_TWO_PT_H_




SC_MODULE (butterfly_two_pt)
{
	sc_in <sc_int<32> > x_r,y_r, x_im,y_im;
	sc_out <sc_int<32> > result_0_r,result_1_r,result_0_im,result_1_im;

	void butterfly_fft ();

	////////// Signals for Complex ALU//////////
	sc_signal<sc_int<32> > ctrl_1;
//	sc_signal<sc_int<32> > x_r_1;
//	sc_signal<sc_int<32> > x_im_1;
//	sc_signal<sc_int<32> > y_r_1;
//	sc_signal<sc_int<32> > y_im_1;
//	sc_signal<sc_int<32> > z_r_1;
//	sc_signal<sc_int<32> > z_im_1;

	sc_signal<sc_int<32> > ctrl_2;

//	sc_signal<sc_int<32> > z_r_2;
//	sc_signal<sc_int<32> > z_im_2;

	complex_alu *complex_1, *complex_2;




	SC_CTOR (butterfly_two_pt)
	{
		complex_1 = new complex_alu ("comp1");
		complex_2 = new complex_alu ("comp2");

		complex_1 -> ctrl(ctrl_1);
		complex_1 -> x_r (x_r);
		complex_1 -> x_im(x_im);
		complex_1 -> y_r(y_r);
		complex_1 -> y_im(y_im);
		complex_1 -> z_im(result_0_im);
		complex_1 -> z_r(result_0_r);

		complex_2 -> ctrl(ctrl_2);
		complex_2 -> x_r (x_r);
		complex_2 -> x_im(x_im);
		complex_2 -> y_r(y_r);
		complex_2 -> y_im(y_im);
		complex_2 -> z_im(result_1_im);
		complex_2 -> z_r(result_1_r);

		SC_METHOD (butterfly_fft);
		sensitive<<x_r<<y_r<<x_im<<y_im;
	}

	//////// Destructor/////////
	~butterfly_two_pt()
	{
		delete complex_1;
		delete complex_2;
	}
};


#endif /* BUTTERFLY_TWO_PT_H_ */
