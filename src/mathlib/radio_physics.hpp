#pragma once

#include <complex>
#include <cmath>

class RadioPhysics
{
public:
    static int sampleDelayForDistance(double distance);
    static double phaseRotationForDistance(double distance);
    static void complexRotationForDistance(std::complex<double> &valOut, double distance);
    static void complexRotation(std::complex<double> &valOut, double theta);
    static void applyPowerLoss(std::complex<double> &valOut, double distance);
    static double freeSpacePowerLoss(double distance);
};
