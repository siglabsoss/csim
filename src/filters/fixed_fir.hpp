#pragma once

#include <vector>
#include <memory>
#include <stdlib.h>
#include <boost/circular_buffer.hpp>

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <types/circularbuffer.hpp>

struct FixedFIRConfig
{
    size_t  wlCoeff;    // coefficient word length
    size_t  wlDelay;    // delay line word length
    size_t  iwlDelay;   // delay line integer length
    size_t  wlOut;      // output word length
    size_t  iwlOut;     // output integer length
    ssize_t rateChange; // up/down sample factor (positive = upsample, negative
                        // = downsample, 0 = no rate change)
};

template<class COEFF_T, class IO_T, class COEFFI_T>
class FixedFIR : public FilterChainElement
{
public:

    FixedFIR(const std::vector<COEFFI_T>& coeffs,
             const FixedFIRConfig       & conf);
    bool input(const filter_io_t& data) override;
    bool output(filter_io_t& data) override;
    void tick() override;

private:

    // methods
    void filter(IO_T & input,
                size_t polyPhaseOffset);

private:

    // members
    std::vector<COEFF_T> m_coeffs;
    CircularBuffer<IO_T> m_delayLine;
    IO_T m_output;
    IO_T m_accum;
    ssize_t m_rateAdj;
    bool    m_outputReady;
    IO_T    m_lastInput;
    bool    m_newInput;
    size_t  m_ticksSinceLastInput;
    size_t  m_filterIteration;
};

typedef class FixedFIR<SLFixPoint, SLFixComplex, double>FixedFirRealCoeff;
typedef class FixedFIR<SLFixPoint, SLFixPoint, double> FixedFirReal;
typedef class FixedFIR<SLFixComplex, SLFixComplex, ComplexDouble> FixedFirComplex;
