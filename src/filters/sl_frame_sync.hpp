#pragma once

#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>
#include <types/circularbuffer.hpp>

class LSFrameSync : public FilterChainElement
{
public:

    LSFrameSync(size_t cpLen,
                size_t numTrainingSym,
                MCS    mcs);

    bool   output(filter_io_t& data) override;
    void   tick(void) override;

    double getTimingMetric() const;
    size_t getPeakDetectionCount() const;
    size_t getFrameDetectionCount() const;

    void   setOutputTimingMetric(bool flag);

private:

    FixedFIR<SLFixComplex, SLFixComplex> *m_pilotFilt;
};
