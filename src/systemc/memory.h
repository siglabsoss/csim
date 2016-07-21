/*
 * memory.h
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "systemc.h"
#ifndef MEMORY_H_
#define MEMORY_H_

const int WORD_SIZE = 32;
const int ADDR_SIZE = 32;
const int MEM_SIZE = 100;

SC_MODULE(memory)
{
public:

    //sc_uint<32> MEM_SIZE;
    sc_in<bool> en;
    sc_in<bool> read;
    sc_in<bool> write;
    sc_in<bool> clk;
    sc_in<sc_int<ADDR_SIZE> > addr_write;
    sc_in<sc_int<ADDR_SIZE> > addr_read;
    sc_in<sc_int<2 * WORD_SIZE> > data_in;
    sc_out<sc_int<2 * WORD_SIZE> > data_out;

    void prc_memory();
    sc_int<2 * WORD_SIZE> ram[MEM_SIZE];

    SC_CTOR(memory)
    {
        SC_METHOD(prc_memory);
        sensitive << clk.pos();
    }
};

#endif /* MEMORY_H_ */
