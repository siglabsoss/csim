#pragma once

#include <core/parameters.hpp>

//Sample rates defined such that mod sample rate is half of digital sample rate
PARAM_DEFINE(RADIO_DIGITAL_SAMPLERATE, 2.5e8);
PARAM_DEFINE(MOD_SAMPLERATE, 1.25e8);
PARAM_DEFINE(RADIO_CARRIER_FREQ, 9.15e8);
PARAM_DEFINE(PHY_LIGHTSPEED, 3.0e8);
