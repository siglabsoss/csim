#pragma once

#include <complex>
#include <cmath>

#include <types/complexdouble.hpp>

class RadioPhysics
{
public:
    static uint32_t sampleDelayForDistance(double distance);
    static double phaseRotationForDistance(double distance);
    static void complexRotationForDistance(ComplexDouble &valOut, double distance);
    static void complexRotation(ComplexDouble &valOut, double theta);
    static double freeSpacePowerLoss(double distance);
};
