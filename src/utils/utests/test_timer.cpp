#include <test/unit_test.hpp>

#include <utils/filter_timer.hpp>

CSIM_TEST_SUITE_BEGIN(TimerValidation)

CSIM_TEST_CASE(TIMER_DOES_ELAPSE_AND_RESET)
{
    Timer timer;

    timer.reset();
    double start = timer.elapsed();
    usleep(12345);
    double middle = timer.elapsed();
    timer.reset();
    double restart = timer.elapsed();

    BOOST_CHECK(start < middle);
    BOOST_CHECK(restart < middle);
    BOOST_CHECK(middle < 0.02);
}

CSIM_TEST_SUITE_END()
