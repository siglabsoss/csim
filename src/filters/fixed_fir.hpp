#pragma once

#include <vector>
#include <iterator>     // ostream_operator
#include <iomanip> //For setprecision

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>

#include <boost/circular_buffer.hpp>

class FixedFIR : public FilterChainElement
{
public:
    FixedFIR(int N, FixedComplex2<16, 1>* tap);

    bool input(const filter_io_t &data) override;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    bool output(filter_io_t &data) override;

    void tick() override;

private: //methods
    void reset();
    FixedComplex2<16, 1> filter(FixedComplex2<16, 1> &input);

private: //members
    std::vector< FixedComplex2<16, 1> >   m_taps;
    boost::circular_buffer< FixedComplex2<16, 1> > m_bench;

    FixedComplex2<16, 1>                  m_output;
};
