/*
 * driver.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: ubuntu
 */
#include "driver.h"

void driver :: prc_driver()
{
	//sc_clock clk ("clk",10,0.5,1,false);
	N_STAGES.write(8);
	int data_in[8] = {5,6,8,-5,6,12,10,9};
//	auto init = std::initializer_list<int>({5,6,8,-5,6,12,10,9});
//	std::copy(init.begin(), init.end(), data_in);

	int i = 0;
	rst.write(0);
	wait(10,SC_NS);
	rst.write(1);
	clk.write(0);
	wait(10,SC_NS);
	clk.write(1);
	wait(10,SC_NS);
	clk.write(0);
	wait(10,SC_NS);
	clk.write(1);
	wait(10,SC_NS);
	clk.write(0);
	rst.write(0);
	wait(10,SC_NS);
	//wait();


	while(i<100)
	{
		clk.write(1);

		x_in_im.write(0);
		x_in_r.write(data_in[i]);
		wait(10,SC_NS);
		clk.write(0);
		wait(10,SC_NS);
		i = i + 1;
	}
}
