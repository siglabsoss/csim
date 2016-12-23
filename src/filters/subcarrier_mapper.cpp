#include <filters/subcarrier_mapper.hpp>

#include <cassert>

SubcarrierMapper::SubcarrierMapper(MCS mcs) :
        FilterChainElement("SubcarrierMapper", mcs.getNumSubCarriers() * 2),
        m_mcs(mcs),
        m_state(WAITING_FOR_INPUT),
        m_output(),
        m_shouldOutput(false),
        m_shortPreamble(mcs.getNumSubCarriers()),
        m_longPreamble(mcs.getNumSubCarriers()),
        m_preambleIdx(0),
        m_subSymbolCount(0)
{
    initializePreamble();
}

void SubcarrierMapper::initializePreamble()
{
    size_t preambleSize = m_shortPreamble.size(); //both preamble vectors are the same size
    for (size_t i = 0; i < preambleSize; ++i) {
        m_shortPreamble[i].setFormat(FFT_INPUT_FORMAT);
        m_shortPreamble[i].set(0.0, 0.0);
        m_longPreamble[i].setFormat(FFT_INPUT_FORMAT);
        m_longPreamble[i].set(0.0, 0.0);
    }

    //XXX load up the appropriate subchannels with the pseudonoise patterns
}

SLFixComplex SubcarrierMapper::getNextShortPreambleSymbol(bool &finished)
{
    size_t idx = m_preambleIdx;
    finished = false;
    if (++m_preambleIdx >= m_shortPreamble.size()) {
        m_preambleIdx = 0;
        finished = true;
    }
    return m_shortPreamble[idx];
}

SLFixComplex SubcarrierMapper::getNextLongPreambleSymbol(bool &finished)
{
    size_t idx = m_preambleIdx;
    finished = false;
    if (++m_preambleIdx >= m_longPreamble.size()) {
        m_preambleIdx = 0;
        finished = true;
    }
    return m_longPreamble[idx];
}

void SubcarrierMapper::tick()
{
    switch(m_state) {
        case WAITING_FOR_INPUT:
            if (m_fifo.size() > 0) {
                m_state = OUTPUT_SHORT_PREAMBLE;
                assert(m_preambleIdx == 0);
            }
            break;
        case OUTPUT_SHORT_PREAMBLE:
        {
            bool sent_short_preamble;
            m_output = getNextShortPreambleSymbol(sent_short_preamble);
            m_shouldOutput = true;
            if (sent_short_preamble) {
                m_state = OUTPUT_LONG_PREAMBLE;
                assert(m_preambleIdx == 0);
            }
            break;
        }
        case OUTPUT_LONG_PREAMBLE:
        {
            bool sent_long_preamble;
            m_output = getNextLongPreambleSymbol(sent_long_preamble);
            m_shouldOutput = true;
            if (sent_long_preamble) {
                m_state = OUTPUT_SYMBOLS;
                assert(m_preambleIdx == 0);
                assert(m_subSymbolCount == 0);
            }
            break;
        }
        case OUTPUT_SYMBOLS:
            if (m_fifo.size() > 0) {
                filter_io_t sample = m_fifo.front();
                m_fifo.pop_front();
                assert(sample.type == IO_TYPE_COMPLEX_FIXPOINT);
                m_output = sample;
                m_shouldOutput = true;
                if (++m_subSymbolCount == (m_mcs.getNumActiveSubCarriers() * m_mcs.getNumOFDMSymbols())) {
                    m_subSymbolCount = 0;
                    m_state = WAITING_FOR_INPUT;
                }
            }
            if (m_fifo.size() == 0) {
                m_state = WAITING_FOR_INPUT;
            }
            break;
    }
}

bool SubcarrierMapper::output(filter_io_t &data)
{
    bool didOutput = m_shouldOutput;
    if (m_shouldOutput) {
        m_shouldOutput = false;
        data = m_output;
    }
    return didOutput;
}
