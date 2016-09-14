/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * and the block floating point technique to maximize precision.
 */

#pragma once

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <memory>

class BFPFFT : public FilterChainElement
{
public:
    BFPFFT(size_t N, bool inverse);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;
private:
    FixedComplexNorm16 getTwiddleFactor(size_t stage, size_t n) const;
    void dit();

    template <typename COMPLEX_T>
    void shiftFixedComplex(COMPLEX_T &val, ssize_t shiftBits);
    void shiftOutput(ssize_t shiftAmount);
    void shiftStage(ssize_t shiftAmount);
    ssize_t calculateShiftAmountForStage(size_t stage);
    void updateMaxValueForStage(size_t stage, const FixedComplex &val);

    size_t                           m_numStages;
    std::vector<FixedComplex1_31>    m_inputs;
    std::vector< ComplexInt >        m_outputs;
    std::vector<FixedComplexNorm16>  m_twiddleFactors;
    bool                             m_outputValid;
    size_t                           m_outputIdx;
    size_t                           m_inputIdx;

    std::vector<uint64_t>            m_maxValuePerStage;
    ssize_t                          m_scaleExp;
    bool                             m_inverse;
};
