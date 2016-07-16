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
	sc_signal <sc_int<32> > x_in_r, x_in_im,ready_in_2,ready_in_1,ready_out_1,ready_in_3;
	sc_signal <sc_int<32> > z_r_1,z_im_1,z_r_2,z_im_2,z_r,z_im;

	syscfft m1 ("FFT_first_stage", 8);

	m1.clk(clk);
	m1.rst(rst);
	m1.x_in_r(x_in_r);
	m1.x_in_im(x_in_im);
	m1.z_r(z_r_1);
	m1.z_im(z_im_1);
	m1.ready_in(ready_in_1);
	m1.ready_out(ready_out_1);

	syscfft m2 ("FFT_second_stage", 4);
	ready_in_3.write(1);
	m2.clk(clk);
	m2.rst(rst);
	m2.x_in_r(z_r_1);
	m2.x_in_im(z_im_1);
	m2.z_r(z_r_2);
	m2.z_im(z_im_2);
	m2.ready_in(ready_in_2);
	m2.ready_out(ready_in_1);

	syscfft m3 ("FFT_third_stage", 2);

	m3.clk(clk);
	m3.rst(rst);
	m3.x_in_r(z_r_2);
	m3.x_in_im(z_im_2);
	m3.z_r(z_r);
	m3.z_im(z_im);
	m3.ready_in(ready_in_3);
	m3.ready_out(ready_in_2);

	driver d1("GetWaveForms");
	//d1.N_STAGES(N_STAGES);
	d1.clk(clk);
	d1.rst(rst);
	d1.x_in_r(x_in_r);
	d1.x_in_im(x_in_im);

	monitor mo1("Monitor_Waveforms");
	mo1<<N_STAGES<<clk<<rst<<x_in_r<<x_in_im<<z_r<<z_im;

	sc_trace_file *wf = sc_create_vcd_trace_file("counter");

	sc_trace(wf, clk, "clk");
	sc_trace(wf, rst, "rst");
	sc_trace(wf, x_in_r, "x_in_r");
	sc_trace(wf, x_in_im, "x_in_im");
	sc_trace(wf, z_r_1, "z_r_1");
	sc_trace(wf, z_im_1, "z_im_1");
	sc_trace(wf, ready_in_1, "ready_in_1");
	sc_trace(wf, ready_out_1, "ready_out_1");
	sc_trace(wf, z_r_2, "z_r_2");
	sc_trace(wf, z_im_2, "z_im_2");
	sc_trace(wf, ready_in_2, "ready_in_2");
//	sc_trace(wf, ready_in_1, "ready_in_1");
	sc_trace(wf, ready_in_3, "ready_in_3");
//	sc_trace(wf, ready_in_1, "ready_in_1");



	sc_start(99000, SC_NS);

	return(0);

}
