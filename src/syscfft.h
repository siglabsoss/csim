/*
 * syscfft.h
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "butterfly_two_pt.h"
#include "twiddler.h"
#include "memory.h"

#ifndef SYSCFFT_H_
#define SYSCFFT_H_

//const int WORD_SIZE = 32;
//const int ADDR_SIZE = 32;
//const int N_STAGES = 8;


SC_MODULE(syscfft)
{
	sc_in<sc_int<32> > N_STAGES;
	sc_in <bool> clk,rst;
	sc_in <sc_int<32> > x_in_r, x_in_im;
	sc_out <sc_int<32> > z_r,z_im;

	////////// Memory Signals //////////
	sc_signal <sc_uint<32> > MEM_SIZE;
	sc_signal <bool> en, read, write;
	sc_signal <sc_int<ADDR_SIZE> > addr_read, addr_write;
	sc_signal <sc_int<2*WORD_SIZE> > data_in;
	sc_signal <sc_int<2*WORD_SIZE> > data_out;

	////////// Twiddler Signals //////////
	sc_signal <sc_int<32> > N, k;
	sc_signal <sc_int<32> > W_r,W_im;

	////////// Butterfly Signals //////////
	sc_signal <sc_int<32> > x_r,x_im,y_r,y_im;
	sc_signal <sc_int<32> > result_0_r,result_0_im, result_1_r,result_1_im;

	memory *memory_stage_ptr;
	twiddler *twiddler_stage_ptr;
	butterfly_two_pt *butterfly_stage_ptr;

#define FFT_STATE_INITIAL (0)
#define FFT_STATE_READ    (1)
#define FFT_STATE_OUTPUT  (2)


	sc_signal<uint8_t> melay_state, next_state;
	void prc_state();
	void prc_output();

	SC_CTOR (syscfft)
	{
		memory_stage_ptr = new memory("memory_stage");
		//memory_stage_ptr -> MEM_SIZE (999);
		memory_stage_ptr -> en (en);
		memory_stage_ptr -> read (read);
		memory_stage_ptr -> write (write);
		memory_stage_ptr -> clk (clk);
		memory_stage_ptr -> addr_read (addr_read);
		memory_stage_ptr -> addr_write (addr_write);
		memory_stage_ptr -> data_in (data_in);
		memory_stage_ptr -> data_out (data_out);

		twiddler_stage_ptr = new twiddler("twiddler_stage");
		twiddler_stage_ptr -> N (N);
		twiddler_stage_ptr -> k (k);
		twiddler_stage_ptr -> W_r (W_r);
		twiddler_stage_ptr -> W_im (W_im);

		butterfly_stage_ptr = new butterfly_two_pt("butterfly_stage");
		butterfly_stage_ptr -> x_r (x_r);
		butterfly_stage_ptr -> x_im (x_im);
		butterfly_stage_ptr -> y_r (y_r);
		butterfly_stage_ptr -> y_im (y_im);
		butterfly_stage_ptr -> result_0_r (result_0_r);
		butterfly_stage_ptr -> result_0_im (result_0_im);
		butterfly_stage_ptr -> result_1_r (result_1_r);
		butterfly_stage_ptr -> result_1_im (result_1_im);

		SC_METHOD (prc_state);
		sensitive<<clk.pos()<<rst.pos(); //sensitive at pos edge of clk and reset
		SC_METHOD(prc_output);
		sensitive<<melay_state<<next_state;

		sc_trace_file *wf = sc_create_vcd_trace_file("counter");
		// Dump the desired signals
		sc_trace(wf, clk, "clk");
		sc_trace(wf, rst, "rst");
		sc_trace(wf,x_in_r, "x_in_r");
		sc_trace(wf, x_in_im, "x_in_im");
		sc_trace(wf, z_r, "z_r");
		sc_trace(wf, z_im, "z_im");
		sc_trace(wf, melay_state, "melay_state");
		sc_trace(wf, next_state, "next_state");
		sc_trace(wf, en, "en");
		sc_trace(wf, read, "read");
		sc_trace(wf, write, "write");
		sc_trace(wf, addr_read, "addr_read");
		sc_trace(wf, addr_write, "addr_write");
		sc_trace(wf, data_in, "data_in");
		sc_trace(wf, data_out, "data_out");
		sc_trace(wf, N, "N");
		sc_trace(wf, k, "k");
		sc_trace(wf, W_r, "W_r");
		sc_trace(wf, W_im, "W_im");
		sc_trace(wf, x_r, "x_r");
		sc_trace(wf, x_im, "x_im");
		sc_trace(wf, y_r, "y_r");
		sc_trace(wf, y_im, "y_im");
		sc_trace(wf, result_0_r, "result_0_r");
		sc_trace(wf, result_0_im, "result_0_im");
		sc_trace(wf, result_1_r, "result_1_r");
		sc_trace(wf, result_1_im, "result_1_im");
		sc_trace(wf, N_STAGES, "N_STAGES");

	}

	~syscfft()
	{
		delete memory_stage_ptr;
		delete twiddler_stage_ptr;
		delete butterfly_stage_ptr;
	}

};

#endif /* SYSCFFT_H_ */