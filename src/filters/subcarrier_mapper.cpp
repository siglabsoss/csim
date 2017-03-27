#include <filters/subcarrier_mapper.hpp>

#include <cassert>
#include <random>

SubcarrierMapper::SubcarrierMapper(MCS mcs) :
    FilterChainElement("SUBCARRIER_MAPPER", mcs.getNumSubCarriers() * 4),
    m_mcs(mcs),
    m_state(WAITING_FOR_INPUT),
    m_output(),
    m_shouldOutput(false),
    m_preamble(mcs.getNumSubCarriers() * 2),
    m_preambleIdx(0),
    m_subSymbolCount(0)
{
    initializePreamble();
}

void SubcarrierMapper::initializePreamble()
{
    size_t symbolSize = m_preamble.size() / 2; // both preamble vectors are

    // the same size

    for (size_t i = 0; i < m_preamble.size(); ++i) {
        m_preamble[i].setFormat(FFT_INPUT_FORMAT);
        m_preamble[i].set(0.0, 0.0);
    }
    auto g =
        std::bind(std::uniform_int_distribution<unsigned>(0, 1), std::mt19937());

    for (size_t i = 0; i < (symbolSize >> 1); ++i) {
        int val = (g() << 1) - 1;
        m_preamble[i * 2].real(static_cast<double>(val) * M_SQRT2);
    }

    for (size_t i = 0; i < (symbolSize >> 2); ++i) {
        int val = (g() << 1) - 1;
        m_preamble[symbolSize + i * 4].real(static_cast<double>(val) * 2.0);
    }

    //    for (size_t i = 0; i < preambleSize; ++i) {
    //        std::cout << i << ": " << m_preamble[i] << std::endl;
    //    }
}

SLFixComplex SubcarrierMapper::getNextPreambleSymbol(bool& finished)
{
    size_t idx = m_preambleIdx;

    finished = false;

    if (++m_preambleIdx >= m_preamble.size()) {
        m_preambleIdx = 0;
        finished      = true;
    }
    return m_preamble[idx];
}

void SubcarrierMapper::tick()
{
    switch (m_state) {
    case WAITING_FOR_INPUT:
    {
        if (m_fifo.size() > 0) {
            m_state = OUTPUT_PREAMBLE;
            assert(m_preambleIdx == 0);
        }
        break;
    }

    case OUTPUT_PREAMBLE:
    {
        bool sent_preamble;
        m_output       = getNextPreambleSymbol(sent_preamble);
        m_shouldOutput = true;

        if (sent_preamble) {
            m_state = OUTPUT_SYMBOLS;
            assert(m_preambleIdx == 0);
            assert(m_subSymbolCount == 0);
        }
        break;
    }

    case OUTPUT_SYMBOLS:
    {
        if (m_fifo.size() > 0) {
            filter_io_t sample = m_fifo.front();
            m_fifo.pop_front();
            assert(sample.type == IO_TYPE_COMPLEX_FIXPOINT);
            m_output       = sample;
            m_shouldOutput = true;

            if (++m_subSymbolCount ==
                (m_mcs.getNumActiveSubCarriers() * m_mcs.getNumOFDMSymbols())) {
                m_subSymbolCount = 0;

                if (m_fifo.size() > 0) {
                    // we already have another frame so jump straight to
                    // preamble so we don't skip a beat
                    m_state = OUTPUT_PREAMBLE;
                } else {
                    m_state = WAITING_FOR_INPUT;
                }
            }
        }
        break;
    }
    }
}

bool SubcarrierMapper::output(filter_io_t& data)
{
    bool didOutput = m_shouldOutput;

    if (m_shouldOutput) {
        m_shouldOutput = false;
        data           = m_output;
    }
    return didOutput;
}
