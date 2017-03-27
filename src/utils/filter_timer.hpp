#pragma once

#include <utils/timer.hpp>
#include <types/circularbuffer.hpp>
#include <cstdlib>

static constexpr size_t TIMING_HISTORY_LENGTH = 1000;

class FilterTimer
{
public:

    FilterTimer();
    void   start();
    void   stop();
    double getAverage() const;
    bool   isStarted() const;

private:

    CircularBuffer<double> m_timings;
    double                 m_sum;
    Timer m_timer;
    bool  m_started;
};
