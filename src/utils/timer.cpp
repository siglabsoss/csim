#include <utils/timer.hpp>

Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    (void)gettimeofday(&m_startTime, nullptr);
}

double Timer::elapsed() const
{
    struct timeval now, result;

    (void)gettimeofday(&now, nullptr);
    timersub(&now, &m_startTime, &result);
    return result.tv_sec + result.tv_usec * 1.0e-6;
}
