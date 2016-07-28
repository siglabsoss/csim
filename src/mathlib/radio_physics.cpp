#include <mathlib/radio_physics.hpp>
#include <cfloat>
#include <core/parameters.hpp>

int RadioPhysics::sampleDelayForDistance(double distance)
{
    double METERS_PER_TICK;
    param_get("RADIO_METERS_PER_TICK", METERS_PER_TICK);
    return static_cast<uint32_t>(distance / METERS_PER_TICK);
}

double RadioPhysics::phaseRotationForDistance(double distance)
{
    double WAVELENGTH;
    param_get("RADIO_WAVELENGTH", WAVELENGTH);
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

double RadioPhysics::freeSpacePowerLoss(double distance)
{
    double WAVELENGTH;
    param_get("RADIO_WAVELENGTH", WAVELENGTH);
    return pow(4 * M_PI * distance / WAVELENGTH, 2);
}
