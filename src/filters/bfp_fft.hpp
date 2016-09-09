/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * and the block floating point technique to maximize precision.
 */

#pragma once

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>

class BFPFFT : public FilterChainElement
{
public:
    BFPFFT(size_t N);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    ComplexDouble getTwiddleFactor(size_t stage, size_t n) const;
    void dit(size_t baseT, size_t N, size_t stage);

    std::vector<ComplexDouble> m_inputs;
    std::vector<ComplexDouble> m_outputs;
    std::vector<ComplexDouble> m_twiddleFactors;
    bool                            m_outputValid;
    size_t                          m_outputIdx;
    size_t                          m_inputIdx;
};
