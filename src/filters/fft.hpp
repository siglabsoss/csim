/**
 * @file fft.hpp
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * with techniques to minimize loss of precision.
 */

#pragma once

#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <vector>


// this macro enables block floating point algorithm, if undefined, a bit-growth
// scheme is used instead
#undef FFT_DO_BLOCK_FLOATING_POINT

#ifdef FFT_DO_BLOCK_FLOATING_POINT
# define FFT_INPUT_WL    32
#else // ifdef FFT_DO_BLOCK_FLOATING_POINT
# define FFT_INPUT_WL    18
#endif // ifdef FFT_DO_BLOCK_FLOATING_POINT
#define FFT_INPUT_IWL   2

#define FFT_OUTPUT_WL   18

// The best value for the integer width of the output depends on the
// peak-to-average ratio of the
// inputs. Based on that information and the desired tolerable clipping rate
// (frequency of saturation),
// we can determine a proper value. We want the value as low as tolerable so
// that we maintain as much
// precision as possible.
#define FFT_OUTPUT_IWL  FFT_INPUT_IWL + 5

#define FFT_TWIDDLE_FORMAT            18,            1, \
    SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define FFT_INPUT_FORMAT              FFT_INPUT_WL,  FFT_INPUT_IWL, \
    SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
#define FFT_OUTPUT_FORMAT             FFT_OUTPUT_WL, FFT_OUTPUT_IWL, \
    SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE
/**
 * A fixed-point FFT implementation using decimation-in-time (DIT)
 * with techniques to minimize loss of precision. This implementation uses
 * the radix-2 butterfly approach, which enables compute optimizations, but
 * constrains the size of the inputs to be a power of two.
 */
class FFT : public FilterChainElement
{
public:

    /**
     * Construct an FFT filter.
     * @param N the number of input samples to operate on at once. Must be a
     * power of tow.
     * @param inverse if set to true, the filter will perform an inverse
     * transform. That is, a transformation from frequency-domain to
     * time-domain.
     */
    FFT(size_t N, bool inverse);
    bool input(const filter_io_t& data) override;
    bool output(filter_io_t& data) override;
    void tick(void) override;

    /**
     * Print the twiddle factors to the console in CSV format. The twiddle
     * factors are printed in both in scaled integer form as well as decimal
     * form. Twiddle factors are the precomputed sine and cosine values for
     * the angles in discrete increments of N.
     */
    void printTwiddleFactors() const;

    /**
     * Set the fixed-point format of the output. This FFT implementation is
     * designed in such a way that the intermediate fixed-point values will not
     * lose precision or overflow. The output of the final FFT stage is then
     * cast to this output format, which may cause loss of precision or
     * overflow.
     */
    void setOutputFormat(size_t                      wordLength,
                         ssize_t                     intLength,
                         SLFixPoint::quant_mode_t    quantMode,
                         SLFixPoint::overflow_mode_t overflowMode);

private:

    SLFixComplex getTwiddleFactor(size_t stage,
                                  size_t n) const;
    void         execute();

    void         shiftFixedComplex(SLFixComplex& val,
                                   ssize_t       shiftBits);
    void         shiftOutput(ssize_t shiftAmount);
    void         shiftStage(ssize_t shiftAmount);
    ssize_t      calculateShiftAmountForStage(size_t stage);
    void         updateMaxValueForStage(size_t              stage,
                                        const SLFixComplex& val);

    size_t m_numStages;
    std::vector<SLFixComplex> m_inputs;
    std::vector<SLFixComplex> m_outputs;
    std::vector<SLFixComplex> m_twiddleFactors;
    bool   m_outputValid;
    size_t m_outputIdx;
    size_t m_inputIdx;

    std::vector<uint64_t> m_maxValuePerStage;
    ssize_t m_scaleExp;
    bool    m_inverse;
};

std::vector<ComplexDouble>FFTWrap(
    const std::vector<ComplexDouble>& inputs,
    bool                              inverse,
    size_t                            outputWordLength,
    size_t                            outputIntLength);

std::vector<SLFixComplex>FFTWrap(
    const std::vector<SLFixComplex>& inputs,
    bool                             inverse,
    size_t                           outputWordLength,
    size_t                           outputIntLength);
