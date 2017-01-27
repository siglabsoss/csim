#include <filters/subcarrier_mapper.hpp>

#include <cassert>
#include <random>

SubcarrierMapper::SubcarrierMapper(MCS    mcs,
                                   size_t pilotSpacing,
                                   size_t initialPilotOfset,
                                   size_t symbolRep) :
    FilterChainElement("SUBCARRIER_MAPPER", mcs.getNumSubCarriers() * symbolRep *
                       2),
    m_mcs(mcs),
    m_pilotSpacing(pilotSpacing),
    m_initialPilotOffset(initialPilotOfset),
    m_symbolRep(symbolRep),
    m_state(WAITING_FOR_INPUT),
    m_output(),
    m_shouldOutput(false),
    m_preamble(mcs.getNumSubCarriers()),
    m_preambleIdx(0),
    m_subSymbolCount(0)
{
    initializePreamble();
}

size_t SubcarrierMapper::getNumPilotTones() const
{
    size_t numPilots = 0;

    for (size_t n = m_initialPilotOffset; n < m_mcs.getNumSubCarriers() / 2;
         n += m_pilotSpacing) {
        numPilots += 2; // a positive and negative index for each 'n'
    }

    return numPilots;
}

void SubcarrierMapper::initializePreamble()
{
    for (size_t i = 0; i < m_preamble.size(); ++i) {
        m_preamble[i].setFormat(FFT_INPUT_FORMAT);
        m_preamble[i].set(0.0, 0.0);
    }

    auto g =
        std::bind(std::uniform_int_distribution<unsigned>(0, 1), std::mt19937());

    size_t numPilots = getNumPilotTones();

    std::cout << "Using training symbol with " << numPilots <<
    " pilot tones spaced " << m_pilotSpacing <<
    " tones apart with an initial offset of " << m_initialPilotOffset <<
    std::endl;

    double powerScale =
        sqrt(m_mcs.getNumSubCarriers() / static_cast<double>(numPilots));

    for (size_t n = m_initialPilotOffset; n < m_mcs.getNumSubCarriers() / 2;
         n += m_pilotSpacing) {
        // Positive tone
        size_t fftIdx = subcarrierIdxToFFTIdx(n);
        int    val    = (g() << 1) - 1;
        m_preamble[fftIdx].real(static_cast<double>(val) * powerScale);

        // Corresponding negative tone
        fftIdx = subcarrierIdxToFFTIdx(-n);
        val    = (g() << 1) - 1;
        m_preamble[fftIdx].real(static_cast<double>(val) * powerScale);
    }
}

size_t SubcarrierMapper::subcarrierIdxToFFTIdx(ssize_t subcarrierIdx) const
{
    if (subcarrierIdx >= 0) {
        return subcarrierIdx;
    }
    subcarrierIdx = m_mcs.getNumSubCarriers() + subcarrierIdx;
            assert(subcarrierIdx >= 0);
    return static_cast<size_t>(subcarrierIdx);
}

SLFixComplex SubcarrierMapper::getNextPreambleSymbol(bool& finished)
{
    size_t idx = m_preambleIdx % m_preamble.size();

    finished = false;

    if (++m_preambleIdx >= m_preamble.size() * m_symbolRep) {
        std::cout << "SENT " << m_preambleIdx <<
        " PREAMBLE SYMBOLS, OUTPUTTING DATA SYMBOLS" << std::endl;
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
