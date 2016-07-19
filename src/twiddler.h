/*
 * twiddler.h

 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */
#include "complex_alu.h"
#include <iostream>
#include <cmath>

#ifndef TWIDDLER_H_
#define TWIDDLER_H_

//const float pi = 3.14159265359;
//const int a = pow(2,15);

SC_MODULE (twiddler)
{
    sc_signal<sc_int<32> > pi_in, ctrl_1, ctrl_2, theta, N_s, k_s;
    complex_alu *complex_1, *complex_2; //,*complex_3;
    //sc_signal<sc_int<32> > theta;
    sc_in<sc_int<32> > N, k;
    sc_out<sc_int<32> > W_r, W_im;
    sc_int<32> W_cos, W_sin;
    //int scale;
    void twiddler_comp();
    sc_signal<sc_int<32> > zero, result_0_im, result_0_r, result_1_im;

    SC_CTOR (twiddler)
    {
        complex_1 = new complex_alu("mult_1");
        complex_2 = new complex_alu("divide");
//		complex_3 = new complex_alu ("divide");

        complex_1->ctrl(ctrl_1);
        complex_1->x_r(pi_in);
        complex_1->x_im(zero);
        complex_1->y_r(k_s);
        complex_1->y_im(zero);
        complex_1->z_im(result_0_im);
        complex_1->z_r(result_0_r);

        complex_2->ctrl(ctrl_2);
        complex_2->x_r(result_0_r);
        complex_2->x_im(zero);
        complex_2->y_r(N_s);
        complex_2->y_im(zero);
        complex_2->z_im(result_1_im);
        complex_2->z_r(theta);

        //sc_trace_file *wf = sc_create_vcd_trace_file("twiddler");
        //sc_trace(wf, ctrl_1, "ctrl_1");
        ///sc_trace(wf, pi_in, "pi_in");
//		sc_trace(wf, N, "N");
        //sc_trace(wf, result_0_r, "result_0_r");
        //sc_trace(wf, ctrl_2, "ctrl_2");
//		sc_trace(wf, theta, "theta");
        //sc_trace(wf, result_0_im, "result_0_im");
        SC_METHOD(twiddler_comp);
        sensitive(k);
    }
    ~twiddler()
    {
        delete complex_1;
        delete complex_2;
    }
};

#endif /* TWIDDLER_H_ */
