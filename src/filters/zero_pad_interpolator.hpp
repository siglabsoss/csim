#pragma once

#include <core/filter_chain_element.hpp>

class ZeroPadInterpolator : public FilterChainElement
{
public:
    ZeroPadInterpolator(size_t chunkSize);

    /**
     * Input is expected to be an FFT in fixed point form
     */
    bool input(const filter_io_t &data) override;

    /**
     * Output is a fixed point complex number representing an upsampled FFT with zero padding
     */
    bool output(filter_io_t &data) override;


    void tick(void) override;

private:
    std::vector < FixedComplex32 > m_inputBuffer;
    std::vector < FixedComplex32 > m_outputBuffer;
    bool                           m_shouldOutput;
    size_t                         m_outputIdx;
};
