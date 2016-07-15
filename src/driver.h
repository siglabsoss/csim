/*
 * driver.h
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */
#include <systemc.h>
#ifndef DRIVER_H_
#define DRIVER_H_

SC_MODULE(driver)
{
	sc_out<sc_int<32> > N_STAGES;
	sc_out <bool> clk,rst;
	sc_out <sc_int<32> > x_in_r, x_in_im;

	void prc_driver();

	SC_CTOR (driver)
	{
		SC_THREAD(prc_driver);
		//sensitive<<clk.pos()<<rst.pos();
	}

};



#endif /* DRIVER_H_ */
