#include <systemc/butterfly_two_pt.hpp>

void butterfly_two_pt::butterfly_fft()
{
//	result_0_r = z_r_1;
//	result_0_im = z_im_1;
//
//	result_1_r = z_r_2;
//	result_1_im = z_im_2;
//
    ctrl_1.write(0);
    ctrl_2.write(1);

}
