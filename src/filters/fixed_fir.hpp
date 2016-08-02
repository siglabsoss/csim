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
    FixedFIR(int N, FixedComplex<16>* tap);

    bool input(const filter_io_t &data) override;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    bool output(filter_io_t &data) override;

    void tick() override;

private: //methods
    void reset();
    FixedComplex<16> filter(FixedComplex<16> &input);

private: //members
    std::vector< FixedComplex<16> >   m_taps;
    boost::circular_buffer< FixedComplex<16> > m_bench;

    FixedComplex<16>                  m_output;
};
