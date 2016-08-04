#include <test/unit_test.hpp>

#include <mathlib/cordic.hpp>
#include <iostream>

using namespace std;
CSIM_TEST_SUITE_BEGIN(CordicSuite)

CSIM_TEST_CASE(CordicFunctionality)
{
    cordic c;
    cordic_complex_t cosine;
    cordic_complex_t sine;

    //TODO write real test
    for (cordic_theta_t i = 0; i < 4 * M_PI; i = i + (4 * M_PI / 180.0)) {
        c.calculate(i, sine, cosine);
    }
}
CSIM_TEST_SUITE_END()
