#include <test/unit_test.hpp>

#include <cfloat>

#include <mathlib/radio_physics.hpp>
#include <core/parameters.hpp>

//parameters used by the system under test. defined here so that changes to the app
//parameters doesn't unnecessarily break the test
PARAM_DEFINE(RADIO_CARRIER_FREQ, 9.15e8);
PARAM_DEFINE(PHY_LIGHTSPEED, 3.0e8);
PARAM_DEFINE(RADIO_DIGITAL_SAMPLERATE, 250000000l);

CSIM_TEST_SUITE_BEGIN(RadioPhysicsSuite)

CSIM_TEST_CASE(SAMPLE_DELAY_CALCULATION)
{
    std::vector<std::pair<double, int> > expectedResults;

    expectedResults.push_back(std::pair<double, int>(10.0,                  static_cast<int>(8.339102394)));
    expectedResults.push_back(std::pair<double, int>(0.0,                   static_cast<int>(0.0)));
    expectedResults.push_back(std::pair<double, int>(1564798653.13352694,   static_cast<int>(1303998877.61)));

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
    std::vector<std::pair<double, ComplexDouble > > expectedResults;

    expectedResults.push_back(std::pair<double, ComplexDouble >(0, ComplexDouble(1.0, 0.0)));
    expectedResults.push_back(std::pair<double, ComplexDouble >(M_PI / 2, ComplexDouble(0.0, 1.0)));
    expectedResults.push_back(std::pair<double, ComplexDouble >(M_PI, ComplexDouble(-1.0, 0.0)));
    expectedResults.push_back(std::pair<double, ComplexDouble >(3 * M_PI / 2, ComplexDouble(0.0, -1.0)));
    expectedResults.push_back(std::pair<double, ComplexDouble >(2 * M_PI, ComplexDouble(1.0, 0.0)));

    for (auto it = expectedResults.begin(); it != expectedResults.end(); it++) {
        ComplexDouble sample(1.0, 0.0);
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
        ComplexDouble sample(2.0e20, 2.0e20);
        double fspl = RadioPhysics::freeSpacePowerLoss(it->first);
        BOOST_CHECK_CLOSE(fspl, it->second, DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
