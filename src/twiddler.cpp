/*
 * twiddler.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: ubuntu
 */

#include "twiddler.h"


void twiddler::twiddler_comp()
{

	scale = a/1000.0;

	theta = (2 * pi/N.read()) * k.read();
	W_cos = cos(theta);
	W_sin = -sin(theta);

	W_cos = W_cos * scale;
	W_sin = W_sin * scale;

	W.real.write() = W_cos;
	W.imag.write() = W_sin;
}


