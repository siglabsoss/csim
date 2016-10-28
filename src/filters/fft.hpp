/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * and the block floating point technique to maximize precision.
 */

#pragma once

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <vector>


//this macro enables block floating point algorithm, if undefined, a bit-growth scheme is used instead
#undef FFT_DO_BLOCK_FLOATING_POINT

#ifdef FFT_DO_BLOCK_FLOATING_POINT
#  define FFT_INPUT_WL    32
#else
#  define FFT_INPUT_WL    18
#endif
#define FFT_INPUT_IWL   2

#define FFT_OUTPUT_WL   32

//The best value for the integer width of the output depends on the peak-to-average ratio of the
//inputs. Based on that information and the desired tolerable clipping rate (frequency of saturation),
//we can determine a proper value. We want the value as low as tolerable so that we maintain as much
//precision as possible.
#define FFT_OUTPUT_IWL  FFT_INPUT_IWL + 6

#define FFT_TWIDDLE_FORMAT            18,            1,              SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define FFT_INPUT_FORMAT              FFT_INPUT_WL,  FFT_INPUT_IWL,  SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define FFT_OUTPUT_FORMAT             FFT_OUTPUT_WL, FFT_OUTPUT_IWL, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

class FFT : public FilterChainElement
{
public:
    FFT(size_t N, bool inverse);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

    void printTwiddleFactors() const;
private:
    SLFixComplex getTwiddleFactor(size_t stage, size_t n) const;
    void dit();

    void shiftFixedComplex(SLFixComplex &val, ssize_t shiftBits);
    void shiftOutput(ssize_t shiftAmount);
    void shiftStage(ssize_t shiftAmount);
    ssize_t calculateShiftAmountForStage(size_t stage);
    void updateMaxValueForStage(size_t stage, const SLFixComplex &val);

    size_t                           m_numStages;
    std::vector<SLFixComplex>        m_inputs;
    std::vector<SLFixComplex>        m_outputs;
    std::vector<SLFixComplex>        m_twiddleFactors;
    bool                             m_outputValid;
    size_t                           m_outputIdx;
    size_t                           m_inputIdx;

    std::vector<uint64_t>            m_maxValuePerStage;
    ssize_t                          m_scaleExp;
    bool                             m_inverse;
};
