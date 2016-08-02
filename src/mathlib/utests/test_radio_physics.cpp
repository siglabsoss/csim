#include <test/unit_test.hpp>

#include <cfloat>

#include <mathlib/radio_physics.hpp>
#include <core/parameters.hpp>

PARAM_DEFINE(RADIO_WAVELENGTH, 0.32786885245901637);
PARAM_DEFINE(RADIO_METERS_PER_TICK, 1.19916983);

CSIM_TEST_SUITE_BEGIN(RadioPhysicsSuite)

CSIM_TEST_CASE(SAMPLE_DELAY_CALCULATION)
{
    std::vector<std::pair<double, int> > expectedResults;

    expectedResults.push_back(std::pair<double, int>(10.0,                  static_cast<int>(8.339102394)));
    expectedResults.push_back(std::pair<double, int>(0.0,                   static_cast<int>(0.0)));
    expectedResults.push_back(std::pair<double, int>(1564798653.13352694,   static_cast<int>(1304901619.425771361)));

    for (auto it = expectedResults.begin(); it != expectedResults.end(); it++) {
        BOOST_CHECK_EQUAL(RadioPhysics::sampleDelayForDistance((*it).first), (*it).second);
    }
}

CSIM_TEST_CASE(PHASE_DELAY_CALCULATION)
{
    std::vector<std::pair<double, double> > expectedResults;

    expectedResults.push_back(std::pair<double, double>(0.163934426,                M_PI));
    expectedResults.push_back(std::pair<double, double>(0.163934426 / 2,            M_PI / 2));
    expectedResults.push_back(std::pair<double, double>(3 * 0.163934426 / 2,        3 * M_PI / 2));
    expectedResults.push_back(std::pair<double, double>(327868.85245901637,         0.0));

    for (auto it = expectedResults.begin(); it != expectedResults.end(); it++) {
        BOOST_CHECK_CLOSE(RadioPhysics::phaseRotationForDistance((*it).first), (*it).second, 0.000001);
    }
}

CSIM_TEST_CASE(PHASE_ROTATION_CALCULATION)
{
    std::vector<std::pair<double, std::complex<double> > > expectedResults;

    expectedResults.push_back(std::pair<double, std::complex<double> >(0, std::complex<double>(1.0, 0.0)));
    expectedResults.push_back(std::pair<double, std::complex<double> >(M_PI / 2, std::complex<double>(0.0, 1.0)));
    expectedResults.push_back(std::pair<double, std::complex<double> >(M_PI, std::complex<double>(-1.0, 0.0)));
    expectedResults.push_back(std::pair<double, std::complex<double> >(3 * M_PI / 2, std::complex<double>(0.0, -1.0)));
    expectedResults.push_back(std::pair<double, std::complex<double> >(2 * M_PI, std::complex<double>(1.0, 0.0)));

    for (auto it = expectedResults.begin(); it != expectedResults.end(); it++) {
        std::complex<double> sample(1.0, 0.0);
        RadioPhysics::complexRotation(sample, it->first);
        //for some reason BOOST_CHECK_CLOSE wasn't working, using BOOST_CHECK on the difference instead
        BOOST_CHECK(abs((sample.real() - it->second.real())) < DBL_EPSILON);
        BOOST_CHECK(abs((sample.imag() - it->second.imag())) < DBL_EPSILON);
    }
}

CSIM_TEST_CASE(FREE_SPACE_POWER_LOSS)
{
    std::vector<std::pair<double, double > > expectedResults;

    expectedResults.push_back(std::pair<double, double >(0, 1));
    expectedResults.push_back(std::pair<double, double >(9425, 1 / 130491470289.88402));
    expectedResults.push_back(std::pair<double, double >(15447, 1 / 350515881229.00635));
    expectedResults.push_back(std::pair<double, double >(99, 1 / 14397589.798688836));

    for (auto it = expectedResults.begin(); it != expectedResults.end(); it++) {
        std::complex<double> sample(2.0e20, 2.0e20);
        double fspl = RadioPhysics::freeSpacePowerLoss(it->first);
        BOOST_CHECK_CLOSE(fspl, it->second, DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
