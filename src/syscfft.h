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
	sc_in <FixedComplex<32> > x_in;
	sc_out <FixedComplex<32> > z;

	////////// Memory Signals //////////
	sc_signal <sc_uint<32> > MEM_SIZE;
	sc_signal <bool> en, read, write;
	sc_signal <sc_uint<ADDR_SIZE> > addr_read, addr_write;
	sc_signal <FixedComplex<WORD_SIZE> > data_in;
	sc_signal <FixedComplex<WORD_SIZE> > data_out;

	////////// Twiddler Signals //////////
	sc_signal <sc_int<32> > N, k;
	sc_signal <FixedComplex<32> > W;

	////////// Butterfly Signals //////////
	sc_signal <FixedComplex<32> > x,y;
	sc_signal <FixedComplex<32> > result_0, result_1;

	memory *memory_stage_ptr;
	twiddler *twiddler_stage_ptr;
	butterfly_two_pt *butterfly_stage_ptr;
	enum FFT_STATE
		{
			FFT_STATE_INITIAL,
			FFT_STATE_READ,
			FFT_STATE_OUTPUT,
			FFT_STATE_IDLE
		};
	sc_signal<FFT_STATE> melay_state, next_state;
	void prc_state();
	void prc_output();

	SC_CTOR (syscfft)
	{
		memory_stage_ptr = new memory("memory_stage");
		//memory_stage_ptr -> MEM_SIZE (999);
		memory_stage_ptr -> en (en);
		memory_stage_ptr -> read (read);
		memory_stage_ptr -> write (write);
		memory_stage_ptr -> addr_read (addr_read);
		memory_stage_ptr -> addr_write (addr_write);
		memory_stage_ptr -> data_in (data_in);
		memory_stage_ptr -> data_out (data_out);

		twiddler_stage_ptr = new twiddler("twiddler_stage");
		twiddler_stage_ptr -> N (N);
		twiddler_stage_ptr -> k (k);
		twiddler_stage_ptr -> W (W);

		butterfly_stage_ptr = new butterfly_two_pt("butterfly_stage");
		butterfly_stage_ptr -> x (x);
		butterfly_stage_ptr -> y (y);
		butterfly_stage_ptr -> result_0 (result_0);
		butterfly_stage_ptr -> result_1 (result_1);

		SC_METHOD (prc_state);
		sensitive_pos<<clk<<rst; //sensitive at pos edge of clk and reset
		SC_METHOD(prc_output);
		sensitive<<melay_state<<next_state;
	}

};

#endif /* SYSCFFT_H_ */
