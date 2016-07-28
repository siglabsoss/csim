/*
 * twiddler.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */

#include <systemc/twiddler.hpp>

void twiddler::twiddler_comp()
{

//int 		scale = a/1000.0;

//		sc_int<32> theta;
//		sc_int<32> N_r,k_r;
//									N_r = 8;
//									k_r = 0;
//									theta = (360/N_r) * k_r;
    N_s.write(N.read());
    k_s.write(k.read());
//		W_cos = cos(theta);
//		W_sin = -sin(theta);
//
//		W_cos = W_cos * scale;
//		W_sin = W_sin * scale;
//
//		W_r = W_cos;
//		W_im = W_sin;
    int a[] = {0,1,1,2,2,3,3,4,4,5,6,6,7,7,8,8,9,9,10,10,11,11,12,13,13,14,14,15,15,16,16,16,17,17,18,18,19,19,20,20,21,21,21,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,27,27,28,28,28,29,29,29,29,29,30,30,30,30,30,31,31,31,31,31,31,31,32,32,32,32,32,32,32,32,32,32,32};
//	//	int a = pow(2,15);
//	//	int scale = a/1000.0;
////	zero.write(0);
////	pi_in.write(360);
    ctrl_1.write(2);
    ctrl_2.write(3);
//	//	theta = (2*pi/N)*k;
//	int theta= ((360/N.read()) * k.read());
//	//	sc_int<32> as_cos, as_sin;
//	//	W_cos = cos(theta);
//	//	W_cos = W_cos * scale;
//	//	W_sin = -sin(theta);
//	//	W_sin = W_sin * scale;
    pi_in.write(360);
    W_sin = -a[theta.read()];
    if (theta.read() > 90) {
        W_cos = -a[-90 + theta.read()];
    } else {
        W_cos = a[90 - theta.read()];
    }

    W_r.write(W_cos);
    W_im.write(W_sin);

}

