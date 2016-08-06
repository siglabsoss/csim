#include <mathlib/radio_physics.hpp>
#include <cfloat>
#include <core/parameters.hpp>
#include <utils/utils.hpp>

int RadioPhysics::sampleDelayForDistance(double distance)
{
    double LIGHTSPEED, SAMPLERATE;
    param_get("PHY_LIGHTSPEED", LIGHTSPEED);
    param_get("RADIO_DIGITAL_SAMPLERATE", SAMPLERATE);

    double meters_per_tick = LIGHTSPEED / SAMPLERATE;
    return static_cast<uint32_t>(distance / meters_per_tick);
}

double RadioPhysics::phaseRotationForDistance(double distance)
{
    double LIGHTSPEED, FREQ;
    param_get("PHY_LIGHTSPEED", LIGHTSPEED);
    param_get("RADIO_CARRIER_FREQ", FREQ);

    double wavelength = LIGHTSPEED / FREQ;
    double temp = distance / wavelength;
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
    if (distance < DBL_EPSILON) {
        return 1.0f;
    }
    double LIGHTSPEED, FREQ;
    param_get("PHY_LIGHTSPEED", LIGHTSPEED);
    param_get("RADIO_CARRIER_FREQ", FREQ);

    double wavelength = LIGHTSPEED / FREQ;
    double loss = (1.0 / pow(4 * M_PI * distance / wavelength, 2));
    return bound(0.0, 1.0, loss);
}
