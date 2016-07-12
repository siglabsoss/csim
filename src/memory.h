/*
 * memory.h
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "fixedcomplex.h"
#ifndef MEMORY_H_
#define MEMORY_H_

const int WORD_SIZE = 32;
const int ADDR_SIZE = 32;
const int MEM_SIZE = 100;

SC_MODULE(memory) {
public:

	//sc_uint<32> MEM_SIZE;
	sc_in <bool> en, read, write, clk;
	sc_in <sc_uint<ADDR_SIZE> > addr_read, addr_write;
	sc_in <FixedComplex<WORD_SIZE> > data_in;
	sc_out <FixedComplex<WORD_SIZE> > data_out;

	void prc_memory();
	FixedComplex<WORD_SIZE> ram[MEM_SIZE];

	SC_CTOR(memory)
	{
		SC_METHOD(prc_memory);
		sensitive_pos << clk;
	}
};




#endif /* MEMORY_H_ */
