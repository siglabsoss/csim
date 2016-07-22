/*
 * complex_alu.h
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */
#include <systemc.h>
#ifndef COMPLEX_ALU_H_
#define COMPLEX_ALU_H_

//void bentest()
//{
//
//}

SC_MODULE(complex_alu)
{
    sc_in<sc_int<32> > ctrl;
    sc_in<sc_int<32> > x_r;
    sc_in<sc_int<32> > x_im;
    sc_in<sc_int<32> > y_r;
    sc_in<sc_int<32> > y_im;
    sc_out<sc_int<32> > z_r;
    sc_out<sc_int<32> > z_im;

    void prc_complex_alu();

    SC_CTOR(complex_alu)
    {
//		SC_METHOD(bentest);
        SC_METHOD(prc_complex_alu);
        sensitive << x_r << x_im << y_r << y_im << ctrl;
    }

};

#endif /* COMPLEX_ALU_H_ */
