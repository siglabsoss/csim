/**
 * @file demapper.hpp Perform the inverse of symbol mapping by extracting
 * log-likelihood information from OFDM symbols.
 */
#pragma once

#include <core/filter_chain_element.hpp>
#include <filters/mapper.hpp>
#include <utils/plotter.hpp>
#include <queue>

#define SD_LLR_FORMAT             8, 6,  SLFixPoint::QUANT_RND_HALF_UP, \
    SLFixPoint::OVERFLOW_SATURATE

/**
 * Perform the inverse of symbol mapping by extracting
 * log-likelihood information from OFDM symbols. The log-likelihood ratio (LLR)
 * is a measurement of how likely a given bit was transmitted given the
 * received OFDM symbol. The LLR is defined as the ratio log(P(1) / P(0)) where
 * P(x) is the probability of the given bit to equal x. A negative LLR means
 * that a 1 is more likely than a 0.
 * @see Mapper
 */
class Demapper : public FilterChainElement
{
public:

    /**
     * Construct an inverse mapper
     * @param mcs the given modulation and coding scheme, which is used to
     * understand details about the constellation set.
     * @param hard if true the filter will make 'hard' decisions about each bit.
     * That is it will output a bit value rather than an LLR.
     */
    Demapper(MCS  mcs,
             bool hard = false);
    ~Demapper();

    bool input(const filter_io_t& data) override;

    /**
     * Output either a bit value (hard == true) or an LLR (hard == false).
     * In the case that we're outputting LLRs, they will be output in the
     * fixed point format defined by SD_LLR_FORMAT.
     * @param data will contain the output value. It will be either
     * IO_TYPE_FIXPOINT or IO_TYPE_BIT.
     */
    bool output(filter_io_t& data) override;

    void tick(void) override;

private:

    static double calcLLRIncrement(const ComplexDouble& rxSymbol,
                                   const ComplexDouble& constellation,
                                   double               variance);

    filter_io_t m_value;
    bool m_inputValid;
    constellation_map_t m_constellations; // mapping of symbol -> constellation
                                          // vector
    size_t m_bitsPerSymbol;
    bool   m_hard;                        // hard decision (vs. soft decision)
    double m_awgnVariance;
    std::queue<double> m_llrs;
};
