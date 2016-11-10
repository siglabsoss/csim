#include <channel/sui.hpp>

const std::vector<double> & SUI::getDelays(unsigned int channelNumber)
{
    assert(channelNumber < 7);

    static const std::vector< std::vector<double> > delays = {
            {0.0, 0.4,  0.9},
            {0.0, 0.4,  1.1},
            {0.0, 0.4,  0.9},
            {0.0, 1.5,  4.0},
            {0.0, 4.0,  10.0},
            {0.0, 14.0, 20.0}
    };
    return delays[channelNumber];
}

const std::vector<double> & SUI::getPowers(unsigned int channelNumber)
{
    assert(channelNumber < 7);

    static const std::vector< std::vector<double> > powers = {
            {0.0, -15.0, -20.0},
            {0.0, -12.0, -15.0},
            {0.0, -5.0,  -10.0},
            {0.0, -4.0,  -8.0},
            {0.0, -5.0,  -10.0},
            {0.0, -10.0, -14.0}
    };
    return powers[channelNumber];
}

const std::vector<double> & SUI::getKFactors(unsigned int channelNumber)
{
    assert(channelNumber < 7);

    static const std::vector< std::vector<double> > kFactors = {
            {4.0, 0.0, 0.0},
            {2.0, 0.0, 0.0},
            {1.0, 0.0, 0.0},
            {0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0}
    };
    return kFactors[channelNumber];
}

const std::vector<double> & SUI::getDopplers(unsigned int channelNumber)
{
    assert(channelNumber < 7);
    static const std::vector< std::vector<double> > dopplers = {
            {0.4, 0.3, 0.5},
            {0.2, 0.15, 0.25},
            {0.4, 0.3, 0.5},
            {0.2, 0.15, 0.25},
            {2.0, 1.5, 2.5},
            {0.4, 0.3, 0.5}
    };
    return dopplers[channelNumber];
}

double              SUI::getAntennaCorr(unsigned int channelNumber)
{
    assert(channelNumber < 7);

    const std::vector<double> antennaCorrs = {0.7, 0.5, 0.4, 0.3, 0.5, 0.3};
    return antennaCorrs[channelNumber];
}

double              SUI::getNormFactor(unsigned int channelNumber)
{
    assert(channelNumber < 7);

    const std::vector<double> normFactors = {-0.1771, -0.393, -1.5113, -1.9218, -1.5113, -0.5683};
    return normFactors[channelNumber];
}

//std::vector< std::vector<double> > SUI::fading(unsigned int channelNumber, size_t N, size_t Nfading, size_t Nfosf)
//{
//    const std::vector<double> powerdB  = getPowers(channelNumber);
//    const std::vector<double> kFactor  = getKFactors(channelNumber);
//    const std::vector<double> dopplers = getDopplers(channelNumber);
//    double                    fNorm    = getNormFactor(channelNumber);
//
//
//}
