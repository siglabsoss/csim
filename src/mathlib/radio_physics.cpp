#include <mathlib/radio_physics.hpp>
#include <cfloat>
#include <core/parameters.hpp>
#include <utils/utils.hpp>

uint32_t RadioPhysics::sampleDelayForDistance(double distance)
{
    double LIGHTSPEED;
    int64_t SAMPLERATE;
    param_get("PHY_LIGHTSPEED", LIGHTSPEED);
    param_get("RADIO_DIGITAL_SAMPLERATE", SAMPLERATE);

    double meters_per_tick = LIGHTSPEED / SAMPLERATE;
    uint32_t delay = static_cast<uint32_t>(distance / meters_per_tick);
    if (delay == 0) {
        delay = 1; //special case for close distances, having a minimum of 1 simplifies some logic
    }
    return delay;
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

void RadioPhysics::complexRotationForDistance(ComplexDouble &valOut, double distance)
{
    double theta = RadioPhysics::phaseRotationForDistance(distance);
    RadioPhysics::complexRotation(valOut, theta);
}

void RadioPhysics::complexRotation(ComplexDouble &valOut, double theta)
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
