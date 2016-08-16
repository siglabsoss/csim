#pragma once

#include <core/filter_chain_element.hpp>

class Decimator : public FilterChainElement
{
public:
    Decimator(size_t N, size_t O);

    bool input(const filter_io_t &data) override;

    bool output(filter_io_t &data) override;

    void tick(void) override;

private:
    size_t m_N; //keep every Nth sample
    size_t m_O; //starting offset, which must be less than N
    size_t m_inputCount;
    filter_io_t m_value;
    bool    m_outputReady;
};
