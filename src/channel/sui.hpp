#pragma once

#include <cassert>
#include <vector>

/**
 * http://www.ieee802.org/16/tg3/contrib/802163c-01_29r4.pdf
 *
 * Using Modified Stanford University Interim (SUI) Channel Models
 */
namespace SUI
{
    std::vector<double> getDelays(unsigned int channelNumber);
    std::vector<double> getPowers(unsigned int channelNumber);
    std::vector<double> getKFactors(unsigned int channelNumber);
    std::vector<double> getDopplers(unsigned int channelNumber);
    double              getAntennaCorr(unsigned int channelNumber);
    double              getNormFactor(unsigned int channelNumber);
}

