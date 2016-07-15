/*
 * monitor.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */
#include "monitor.h"
void monitor :: prc_monitor()
{
	cout << "At time "<<sc_time_stamp()<<"::";
	cout << "(x_in_r,x_in_im,clk,rst): ";
	cout <<x_in_r <<x_in_im <<clk <<rst;
	cout << " (z_r and z_im are): " << z_r<<z_im<<endl;
}
