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
    void     filter(SLFixComplex &input);

private: //members
    std::vector<SLFixPoint>                        		    m_coeffs;
    CircularBuffer< SLFixComplex > 	                        m_x;
    SLFixComplex                                          	m_output;
    SLFixComplex                                            m_accum;
};
