#pragma once

#include <vector>
#include <memory>
#include <stdlib.h>
#include <boost/circular_buffer.hpp>

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <types/circularbuffer.hpp>

class FixedFIR : public FilterChainElement
{
public:
    FixedFIR(std::vector<double> coeffs, unsigned coeffWidth);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick() override;

private: //methods
    ComplexInt     filter(FixedComplex16 &input);
    ComplexInt     accumToComplexInt() const;

private: //members
    std::vector< std::unique_ptr<FixPoint> >       		    m_coeffs;
    CircularBuffer< FixedComplex16 > 	                    m_x;
    ComplexInt                                          	m_output;
    std::unique_ptr<FixedComplex>                           m_accum;
    ssize_t                                                 m_inputExp;
};
