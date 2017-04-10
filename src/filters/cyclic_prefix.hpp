/**
 * @file cyclic_prefix.hpp
 *
 * A cyclic prefix generator.
 */

#pragma once

#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>
#include <utils/mcs.hpp>
#include <vector>

/**
 * A cyclic prefix generator.
 *
 * This filter outputs a copy of its input with cpLen samples prepended to it.
 * These cpLen samples are a copy of the last cpLen samples of the input.
 * See https://en.wikipedia.org/wiki/Cyclic_prefix for more information.
 */
class CyclicPrefix : public FilterChainElement
{
public:

    /**
     * Construct a cyclic prefix generator.
     * @param N the number of input samples. This is typically the same as the
     * FFT size in an OFDM system.
     * @param cpLen the number of samples to prefix. This must be less than N
     * and is typically about one-eigth of N.
     * @param ticksPerOutput used to throttle the rate of the output.
     * @param MCS the selected modulation and coding scheme, which is used
     * to set a sensible FIFO size.
     */
    CyclicPrefix(size_t N,
                 size_t cpLen,
                 size_t ticksPerOutput,
                 MCS    mcs);
    bool output(filter_io_t& data) override;
    void tick(void) override;

private:

    size_t m_len;
    size_t m_symbolLen;
    size_t m_inputIdx;
    size_t m_outputIdx;
    size_t m_ticksPerOutput;
    size_t m_ticksSinceOutput;
    bool   m_outputActive;
    std::vector<SLFixComplex> m_outputs;
};
