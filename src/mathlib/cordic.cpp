/*
 * cordic.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: ubuntu
 */

#include <mathlib/cordic.hpp>

cordic::cordic() :
        m_vals(NUM_HARDWIRED_VALUES)
{
    //See Hard-wiring the machine: http://www.qc.cuny.edu/Academics/Degrees/DMNS/Faculty%20Documents/Sultan1.pdf
    for (unsigned int i = 0; i < NUM_HARDWIRED_VALUES; i++) {

        if (i == 0) {
            m_vals[i] = 0.7853981634;
        } else if (i == 1) {
            m_vals[i] = 0.463647609;
        } else if (i == 2) {
            m_vals[i] = 0.2449786631;
        } else if (i == 3) {
            m_vals[i] = 0.1243549945;
        } else if (i == 4) {
            m_vals[i] = 0.06241881;
        } else {
            m_vals[i] = 1.0 / (1 << i);
        }
    } //Calculates and sets values to rotate by
}

int cordic::rotateToFirstQuadrant(cordic_theta_t &theta)
{
    int quad = 0;
    while (theta >= (2 * M_PI)) { //input greater than 2pi
        theta = theta - (2 * M_PI);
    }

    if (theta >= (M_PI_2 + M_PI)) { //4th quadrant 1.5pi
        quad = 4;
        theta = theta - (M_PI_2 + M_PI);
    } else if (theta >= (M_PI)) { //3rd quadrant pi
        quad = 3;
        theta = theta - M_PI;
    } else if (theta >= M_PI_2) { //2nd quadrant 0.5pi
        quad = 2;
        theta = theta - M_PI_2;
    } else { //1st quadrant
        quad = 1;
    }
    return quad;
}

cordic_scalar_t cordic::sin(const cordic_theta_t &theta)
{
    cordic_complex_t sine;
    cordic_complex_t cosine;
    calculate(theta, sine, cosine);
    return sine.real();
}

cordic_scalar_t cordic::cos(const cordic_theta_t &theta)
{
    cordic_complex_t sine;
    cordic_complex_t cosine;
    calculate(theta, sine, cosine);
    return cosine.real();
}

void cordic::calculate(cordic_theta_t theta, cordic_complex_t &sine, cordic_complex_t &cosine)
{
    int quadrant = rotateToFirstQuadrant(theta);

    cordic_complex_t y[2] = {cordic_complex_t(1.0, 0.0), cordic_complex_t(0.0, 1.0)};
    cordic_complex_t c1, c2, c3, c4;
    cordic_theta_t x = 0.0;
    int direction = 0;
    for (unsigned int i = 0; i < NUM_HARDWIRED_VALUES; i++) {
        if (x <= theta) {
            direction = 1; //rotate ccw
            x = x + m_vals[i];
        } else {
            direction = -1; //rotate cw
            x = x - m_vals[i];
        }

        cordic_scalar_t factor(direction/(pow(2,i)));
        c1 = y[0]; //a
        c2 = y[1] * factor; //-b/2^n
        c3 = y[0] * factor; //a/2^n
        c4 = y[1]; //b

        y[0] = c1 - c2; //a + (-b/2^n)
        y[1] = c3 + c4; //a/2^n + b

        //std::cout /*<< "y0 = " << y[0]*/ << "\ty1 = " << y[1] << "\t dir = " << direction << "\tx = " << x << "\tval[i] = " << m_vals[i] << std::endl;
    }

    y[0] = y[0] * cordic_scalar_t(k);
    y[1] = y[1] * cordic_scalar_t(k);

    rotateToQuadrant(y, quadrant);

    //std::cout << quadrant << ", " << y[0].real() << ", " << y[0].imag() << ", " << y[1].real() << ", " << y[1].imag() << ", " << theta << ", " << x << ", " << abs(theta - x) << std::endl;

    clipResults(y);

    cosine  = y[0]; // cosdown / cos = y[0].real, sindown = y[0].imag
    sine    = y[1]; // sinup / sin = y[1].real, cosup = y[1].imag;
}

void cordic::clipResults(cordic_complex_t y[2])
{
    for (int i = 0; i < 2; i++) {
        if (y[i].real() > 1.0) {
            y[i].real(1.0);
        } else if (y[i].real() < -1.0) {
            y[i].real(-1.0);
        }

        if (y[i].imag() > 1.0) {
            y[i].imag(1.0);
        } else if (y[i].imag() < -1.0) {
            y[i].imag(-1.0);
        }
    }
}

void cordic::rotateToQuadrant(cordic_complex_t y[2], int quadrant)
{
    cordic_complex_t temp;
    if (quadrant == 4) {
        temp = y[0];
        y[0] = y[1];
        y[1] = temp * cordic_scalar_t(-1);
    } //swap sin and cosine. sin is negative
    else if (quadrant == 3) {
        y[0] = y[0] * cordic_scalar_t(-1);
        y[1] = y[1] * cordic_scalar_t(-1);
    } //both are negative
    else if (quadrant == 2) {
        temp = y[0];
        y[0] = y[1] * cordic_scalar_t(-1);
        y[1] = temp;
    } else if (quadrant == 1){//swap sin and cosine. cosine is negative
        /* do nothing */
    } else {
        assert(false);
    }
} //swaps cosine and sines or changes signs if necessary.


