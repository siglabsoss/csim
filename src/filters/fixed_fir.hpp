#pragma once

#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <boost/circular_buffer.hpp>

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>

class FixedFIR : public FilterChainElement
{
public:
    FixedFIR(vector<FixedComplex16> tap);

    bool input(const filter_io_t &data) override;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    bool output(filter_io_t &data) override;

    void tick() override;

private: //methods
    void reset();
    FixedComplex16 filter(FixedComplex16 &input);

private: //members
    std::vector< FixedComplex16 >   m_taps;
    boost::circular_buffer< FixedComplex16 > m_bench;

    FixedComplex16                  m_output;
};
