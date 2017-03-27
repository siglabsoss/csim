#pragma once

#include <sys/time.h>

class Timer
{
public:

    Timer();
    void   reset();
    double elapsed() const;

private:

    struct timeval m_startTime;
};
