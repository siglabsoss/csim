/*
 * monitor.h
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */
#include <systemc.h>
#ifndef MONITOR_H_
#define MONITOR_H_

SC_MODULE (monitor)
{
	sc_in<sc_int<32> > N_STAGES;
	sc_in <bool> clk,rst;
	sc_in <sc_int<32> > x_in_r, x_in_im;
	sc_in <sc_int<32> > z_r,z_im;

	void prc_monitor();

	SC_CTOR (monitor)
	{
		SC_METHOD(prc_monitor);
		sensitive << x_in_r << x_in_im << z_r << z_im << clk.pos() << rst << N_STAGES;
	}
};



#endif /* MONITOR_H_ */
