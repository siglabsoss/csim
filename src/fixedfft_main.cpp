/*
 * fixedfft_main.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */

#include "driver.h"
#include "monitor.h"
#include "syscfft.h"

int sc_main(int argc, char* argv[])
{
	sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
			sc_core::SC_DO_NOTHING );
	sc_signal<sc_int<32> > N_STAGES;
	sc_signal <bool> clk,rst;
	sc_signal <sc_int<32> > x_in_r, x_in_im;
	sc_signal <sc_int<32> > z_r,z_im;

	syscfft m1 ("FFT_first_stage");
	//m1<<N_STAGES<<clk<<rst<<x_in_r<<x_in_im<<z_r<<z_im;
	m1.N_STAGES(N_STAGES);
	m1.clk(clk);
	m1.rst(rst);
	m1.x_in_r(x_in_r);
	m1.x_in_im(x_in_im);
	m1.z_r(z_r);
	m1.z_im(z_im);

	driver d1("GetWaveForms");
	d1.N_STAGES(N_STAGES);
	d1.clk(clk);
	d1.rst(rst);
	d1.x_in_r(x_in_r);
	d1.x_in_im(x_in_im);

	monitor mo1("Monitor_Waveforms");
	mo1<<N_STAGES<<clk<<rst<<x_in_r<<x_in_im<<z_r<<z_im;



//	sc_trace(wf, melay_state, "melay_state");
//	sc_trace(wf, enable, "enable");
//	sc_trace(wf, counter_out, "count");


	sc_start(1500, SC_NS);

	return(0);

}
