
#include <filters/depuncture.hpp>

Depuncture::Depuncture(MCS mcs) :
    m_mcs(mcs),
    m_outBitCount(0),
    m_outCodeWordCount(0),
    m_fifo(mcs.getNumCodeWords() * mcs.getCodeWordLength()),
    m_doPuncture(mcs.getNumPuncOrRepBits() >= 0)
{
}

bool Depuncture::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXPOINT);
    m_fifo.push_back(data.fp);
    return true;
}

bool Depuncture::output(filter_io_t &data)
{
    bool didOutput = false;
    if (m_doPuncture) {
        didOutput = handlePuncture(data);
    } else { //repeating bits
        didOutput = handleRepetition(data);
    }
    return didOutput;
}

void Depuncture::tick()
{

}

bool Depuncture::handlePuncture(filter_io_t &data)
{
    bool didOutput = false;
    size_t numPunctureBits = m_mcs.getNumPuncOrRepBits();
    size_t numPunctureBitsPerSymbol = numPunctureBits / m_mcs.getNumOFDMSymbols();
    if (m_outCodeWordCount < (numPunctureBits % m_mcs.getNumOFDMSymbols())) {
        numPunctureBitsPerSymbol += 1;
    }

    if (m_outBitCount < m_mcs.getCodeWordLength() - numPunctureBitsPerSymbol) {
        if (m_fifo.empty()) {
            return false;
        }
        data.type = IO_TYPE_FIXPOINT;
        data.fp.setFormat(m_fifo.front());
        data.fp = m_fifo.front();
        m_fifo.pop_front();
        ++m_outBitCount;
        didOutput = true;
    } else if (m_outBitCount < m_mcs.getCodeWordLength()){
        data.type = IO_TYPE_FIXPOINT;
        data.fp.setFormat(SD_LLR_FORMAT);
        data.fp = 0.0; //equally likely to be '0' or '1'
        ++m_outBitCount;
        didOutput = true;
    }

    if (m_outBitCount == m_mcs.getCodeWordLength()) {
        m_outBitCount = 0;
        if (++m_outCodeWordCount == m_mcs.getNumCodeWords()) {
            m_outCodeWordCount = 0;
        }
    }
    return didOutput;
}

bool Depuncture::handleRepetition(filter_io_t &data)
{
    bool   didOutput = false;
    size_t numRepeatBits = std::abs(m_mcs.getNumPuncOrRepBits());
    size_t numRepeatBitsPerSymbol = numRepeatBits / m_mcs.getNumOFDMSymbols();
    if (m_outCodeWordCount < (numRepeatBits % m_mcs.getNumOFDMSymbols())) {
        numRepeatBitsPerSymbol += 1; //the first rem(numRepeatBits, numOFDMSymbols) get an extra repeat bit
    }

    if (m_fifo.empty()) {
        return false;
    }

    if (m_outBitCount < m_mcs.getCodeWordLength()) {
        //simply pass the codeword through
        data.type = IO_TYPE_FIXPOINT;
        data.fp.setFormat(m_fifo.front());
        data.fp = m_fifo.front();
        m_fifo.pop_front();
        ++m_outBitCount;
        didOutput = true;
    } else if (m_outBitCount < m_mcs.getCodeWordLength() + numRepeatBitsPerSymbol) {
        m_fifo.pop_front();
        ++m_outBitCount;
    }
    if (m_outBitCount == m_mcs.getCodeWordLength() + numRepeatBitsPerSymbol) {
        m_outBitCount = 0;
        if (++m_outCodeWordCount == m_mcs.getNumCodeWords()) {
            m_outCodeWordCount = 0;
        }
    }
    return didOutput;
}
