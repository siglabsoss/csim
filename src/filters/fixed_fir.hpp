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
    struct Config {
        size_t wlCoeff;         //coefficient word length
        size_t wlDelay;         //delay line word length
        size_t iwlDelay;        //delay line integer length
        size_t wlOut;           //output word length
        size_t iwlOut;          //output integer length
        ssize_t rateChange;     //up/down sample factor (positive = upsample, negative = downsample, 0 = no rate change)
    };
    FixedFIR(const std::vector<double> &coeffs, const Config &conf);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick() override;

private: //methods
    void     filter(SLFixComplex &input, size_t polyPhaseOffset);

private: //members
    std::vector<SLFixPoint>                        		    m_coeffs;
    CircularBuffer< SLFixComplex > 	                        m_delayLine;
    SLFixComplex                                          	m_output;
    SLFixComplex                                            m_accum;
    ssize_t                                                 m_rateAdj;
    bool                                                    m_outputReady;
    SLFixComplex                                            m_lastInput;
    bool                                                    m_newInput;
    size_t                                                  m_ticksSinceLastInput;
    size_t                                                  m_filterIteration;
};
