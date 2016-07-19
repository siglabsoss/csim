/*
 * complex_adder.cpp

 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#include "complex_alu.h"

void complex_alu::prc_complex_alu()
{
    if (ctrl.read() == 0) {
        z_r.write(x_r.read() + y_r.read());
        z_im.write(x_im.read() + y_im.read());
    } else if (ctrl.read() == 1) {
        z_r.write(x_r.read() - y_r.read());
        z_im.write(x_im.read() - y_im.read());
    } else if (ctrl.read() == 2) {
        z_r.write(((x_r.read() * y_r.read()) - (x_im.read() * y_im.read())));
        z_im.write((x_im.read() * y_r.read()) + (x_r.read() * y_im.read()));
    } else if (ctrl.read() == 3) {
        z_r.write(
                ((x_r.read() * y_r.read()) + (x_im.read() * y_im.read()))
                        / ((y_r.read() * y_r.read())
                                + (y_im.read() * y_im.read())));
        z_im.write(
                ((x_im.read() * y_r.read()) - (x_r.read() * y_im.read()))
                        / ((y_r.read() * y_r.read())
                                + (y_im.read() * y_im.read())));
    }
}

