#include <utils/filter_timer.hpp>

FilterTimer::FilterTimer() :
    m_timings(TIMING_HISTORY_LENGTH, 0.0),
    m_sum(0.0),
    m_timer(),
    m_started(false)
{}

void FilterTimer::start()
{
    assert(!m_started);
    m_started = true;
    m_timer.reset();
}

void FilterTimer::stop()
{
    assert(m_started);
    m_started = false;

    double newest = m_timer.elapsed();
    double oldest = m_timings.front();

    m_sum += newest - oldest;

    // std::cout << "sum = " << m_sum << " oldest = " << oldest << " newest = "
    //           <<
    // newest << std::endl;
    m_timings.push_back(newest);
}

double FilterTimer::getAverage() const
{
    return m_sum / TIMING_HISTORY_LENGTH;
}

bool FilterTimer::isStarted() const
{
    return m_started;
}
