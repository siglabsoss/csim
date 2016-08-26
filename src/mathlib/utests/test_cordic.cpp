#include <test/unit_test.hpp>

#include <mathlib/cordic.hpp>
#include <iostream>

using namespace std;
CSIM_TEST_SUITE_BEGIN(CordicSuite)

CSIM_TEST_CASE(CordicFunctionality)
{
    static constexpr double AVG_ERR_THRESHOLD = 0.000007;
    static constexpr double MAX_ERR_THRESHOLD = 0.000012;
    cordic c;
    cordic_complex_t cosine;
    cordic_complex_t sine;

    //TODO write real test
    double sin_accum = 0.0;
    double cos_accum = 0.0;
    size_t iterations = 0;
    double max_err = 0.0;
    double theta_at_max = 0.0;
    for (double i = 0; i < 3 * M_PI; i = i + (3 * M_PI / 180.0)) {
        cordic_theta_t theta(i);
        cordic_complex_t sine;
        cordic_complex_t cosine;
        c.calculate(theta, sine, cosine);
        double cordic_sine = sine.real();
        double cordic_cos = cosine.real();
        double libm_sin = sin(i);
        double libm_cos = cos(i);
        double sin_error = abs(cordic_sine - libm_sin);
        double cos_error = abs(cordic_cos - libm_cos);
        //std::cout << "cordic sine = " << sine.real() << "\tsine = " << sin(i) << "\ttheta = " << i << std::endl;


        if (sin_error > max_err) {
            max_err = sin_error;
            theta_at_max = i;
        }
        if (cos_error > max_err) {
            max_err = cos_error;
            theta_at_max = i;
        }

        sin_accum += sin_error;
        cos_accum += cos_error;
        //std::cout << "difference = " << difference << std::endl;
        iterations++;
    }
    double avg_sin_err = sin_accum / iterations;
    double avg_cos_err = cos_accum / iterations;

    std::cout << "Average Error = " << avg_sin_err << ", " << avg_cos_err << std::endl;
    std::cout << "Max Error = " << max_err << std::endl;

    BOOST_CHECK(avg_sin_err < AVG_ERR_THRESHOLD);
    BOOST_CHECK(avg_cos_err < AVG_ERR_THRESHOLD);
    BOOST_CHECK(max_err < MAX_ERR_THRESHOLD);
    //std::cout << "Average error = " << accum / interations << " max = " << max_err << " at theta = " << theta_at_max << std::endl;
}
CSIM_TEST_SUITE_END()
