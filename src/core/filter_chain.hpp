#pragma once

#include <core/filter_chain_element.hpp>
#include <core/publisher.hpp>
#include <utils/filter_timer.hpp>
#include <memory>

class FilterChain : public AbstractSISO<filter_io_t, filter_io_t>
{
public:

    FilterChain();
    FilterChain(FilterChain && other);
    FilterChain(const FilterChainElement& other);
    virtual ~FilterChain() {}

    void         init();

    bool         input(const filter_io_t& data) override;
    bool         output(filter_io_t& data) override;
    void         tick() override;

    FilterChain& operator=(const FilterChainElement& rhs);

    double       getFIFOUtilization() const;
    void         printTimingReport() const;

private:

    // methods
    void publish(FilterChainElement *current);

private:

    std::unique_ptr<FilterChainElement> m_head;
    filter_io_t m_output;
    bool   m_outputReady;
    double m_maxFIFOUtilization;
    bool   m_didInit;
    std::vector<std::unique_ptr<FilterTimer> > m_timers;
};
