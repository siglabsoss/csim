#include <mathlib/radio_physics.hpp>

int RadioPhysics::sampleDelayForDistance(double distance)
{
    constexpr double METERS_PER_TICK = 1.19916983; //TODO make configurable
    return static_cast<uint32_t>(distance / METERS_PER_TICK);
}

double RadioPhysics::phaseRotationForDistance(double distance)
{
    constexpr double WAVELENGTH = 0.32786885245901637; //TODO make configurable
    double temp = distance / WAVELENGTH;
    temp -= floor(temp);
    return temp * 2 * M_PI;
}

void RadioPhysics::complexRotationForDistance(std::complex<double> &valOut, double distance)
{
    double theta = RadioPhysics::phaseRotationForDistance(distance);
    RadioPhysics::complexRotation(valOut, theta);
}

void RadioPhysics::complexRotation(std::complex<double> &valOut, double theta)
{
    //x' = xcos(theta) - ysin(theta)
    //y' = xsin(theta) + ycos(theta)
    double cs = cos(theta);
    double sn = sin(theta);
    double x = valOut.real();
    double y = valOut.imag();
    valOut.real( (x * cs) - (y * sn) );
    valOut.imag( (x * sn) + (y * cs) );
}

void RadioPhysics::applyPowerLoss(std::complex<double> &valOut, double distance)
{

}
