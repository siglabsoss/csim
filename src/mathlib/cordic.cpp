/*
 * cordic.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#include <mathlib/cordic.hpp>

using namespace std;

cordic::cordic() :
        vals(NUM_HARDWIRED_VALUES),
        sign(NUM_HARDWIRED_VALUES),
        quad(-1)
{
    //See Hard-wiring the machine: http://www.qc.cuny.edu/Academics/Degrees/DMNS/Faculty%20Documents/Sultan1.pdf
    for (int i = 0; i < NUM_HARDWIRED_VALUES; i++) {

        if (i == 0) {
            vals[i] = 0.78539816339744830961;
        } else if (i == 1) {
            vals[i] = 0.463647609;
        } else if (i == 2) {
            vals[i] = 0.2449786631;
        } else if (i == 3) {
            vals[i] = 0.1243549945;
        } else if (i == 4) {
            vals[i] = 0.06241881;
        } else if (i == 5) {
            vals[i] = 0.0312398334;
        } else {
            vals[i] = 1.0 / (1 << i);
        }
    } //Calculates and sets values to rotate by
}

void cordic::rotate(cordic_theta_t theta)
{
    //std::cout << "Theta = " << theta;
    while (theta >= cordic_theta_t(2 * M_PI)) { //input greater than 2pi
        theta = theta - (2 * M_PI);
    }

    if (theta >= cordic_theta_t(1.5 * M_PI)) { //4th quadrant 1.5pi
        quad = 4;
        theta = theta - (1.5 * M_PI);
    } else if (theta >= cordic_theta_t(M_PI)) { //3rd quadrant pi
        quad = 3;
        theta = theta - M_PI;
    } else if (theta >= cordic_theta_t(0.5 * M_PI)) { //2nd quadrant 0.5pi
        quad = 2;
        theta = theta - (0.5 * M_PI);
    } else { //1st quadrant
        quad = 1;
    }

    cordic_theta_t x = 0.0; //starting point for rotations

    //std::cout << " in quadrant " << quad << std::endl;
    for (int i = 0; i < NUM_HARDWIRED_VALUES; i++) {
        if (x <= theta) {
            sign[i] = 1;
            x = x + vals[i];
        } //rotate ccw
        else // ( x > theta)
        {
            sign[i] = -1;
            x = x - vals[i];
        } //rotate cw
    }
}

cordic_scalar_t cordic::sin(cordic_scalar_t theta)
{
    cordic_complex_t a(1.0, 0.0);
    cordic_complex_t b(0.0, 0.0);
    cordic_scalar_t cosdown;
    cordic_scalar_t cosup;
    cordic_scalar_t sinup;
    cordic_scalar_t sindown;

    calculate(theta, a, b, &sinup, &sindown, &cosup, &cosdown);
    return sinup;
}

void cordic::calculate(cordic_theta_t theta, cordic_complex_t a, cordic_complex_t b,
        cordic_scalar_t* sinup, cordic_scalar_t* sindown, cordic_scalar_t* cosup,
        cordic_scalar_t* cosdown)
{
    rotate(theta);

    y[0] = a;
    y[1] = b;

    for (int i = 0; i < NUM_HARDWIRED_VALUES; i++) {
        double inverse_2exp = vals[i];
        temp = y[0]; //temporary storage for later calculation
        c1 = y[0];
        c2 = y[1] * cordic_scalar_t(inverse_2exp);

        if (sign[i] == 1) {
            c2 = c2 * cordic_scalar_t(-1.0);
        }

        y[0] = c1 + c2; //Upper value
        c1 = y[1]; //((z[1][1] * y[1][0]) >> 15) z[1][1] = 1 << 15
        c2 = temp * cordic_scalar_t(inverse_2exp);

        if (sign[i] == -1) {
            c2 = c2 * cordic_scalar_t(-1.0);
        }
        y[1] = c2 + c1; //Lower value
    }

    y[0] = y[0] * cordic_scalar_t(k);
    y[1] = y[1] * cordic_scalar_t(k);

    signs();

    cout << "Cosine, " << y[0].real() << ", " << theta << ", " << quad << endl;
	//cout << "Sine, " << y[1].real() << ", " << theta << endl;

    for (int i = 0; i < 2; i++) {
        if (y[i].real() > 1.0) {
            y[i].real(1.0);
        }
        if (y[i].imag() > 1.0) {
            y[i].imag(1.0);
        }
    }

    *cosdown = y[0].real();
    *cosup = y[1].imag();
    *sindown = y[0].imag();
    *sinup = y[1].real();

    return;

}

void cordic::signs()
{
    if (quad == 4) {
        temp = y[0];
        y[0] = y[1];
        y[1] = temp * cordic_scalar_t(-1);
    } //swap sin and cosine. sin is negative
    else if (quad == 3) {
        y[0] = y[0] * cordic_scalar_t(-1);
        y[1] = y[1] * cordic_scalar_t(-1);
    } //both are negative
    else if (quad == 2) {
        temp = y[0];
        y[0] = y[1] * cordic_scalar_t(-1);
        y[1] = temp;
    } else if (quad == 1){//swap sin and cosine. cosine is negative
        /* do nothing */
    } else {
        assert(false);
    }
} //swaps cosine and sign or changes sign if necessary.

cordic::~cordic()
{

}

