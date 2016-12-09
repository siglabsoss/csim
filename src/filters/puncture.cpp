#include <filters/puncture.hpp>
#include <cassert>

Puncture::Puncture(MCS mcs) :
        m_mcs(mcs),
        m_buffer(),
        m_doPuncture(false),
        m_fifo(mcs.getNumCodeWords() * mcs.getCodeWordLength()),
        m_outBitCount(0),
        m_outCodeWordCount(0)
{
    if (mcs.getNumPuncOrRepBits() < 0) { //we need to repeat bits instead of puncture
        size_t numRepeatBits = std::abs(m_mcs.getNumPuncOrRepBits());
        size_t numRepeatBitsPerSymbol = numRepeatBits / m_mcs.getNumOFDMSymbols();
        numRepeatBitsPerSymbol += 1; //one additional bit in case the division above truncated
        m_buffer = std::vector<bool>(numRepeatBitsPerSymbol);
        m_doPuncture = false;
    } else {
        m_doPuncture = true;
    }
}

bool Puncture::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_BIT);
    assert(!m_fifo.full());
    m_fifo.push_back(data.bit);
    return true;
}

bool Puncture::handlePuncture(filter_io_t &data)
{
    size_t numPunctureBits = m_mcs.getNumPuncOrRepBits();
    size_t numPunctureBitsPerSymbol = numPunctureBits / m_mcs.getNumOFDMSymbols();
    bool didOutput = false;
    if (m_outCodeWordCount < (numPunctureBits % m_mcs.getNumOFDMSymbols())) {
        numPunctureBitsPerSymbol += 1;
    }

    if (m_outBitCount < m_mcs.getCodeWordLength() - numPunctureBitsPerSymbol) {
        //simply pass the codeword through
        data.type = IO_TYPE_BIT;
        data.bit = m_fifo.front();
        m_fifo.pop_front();
        ++m_outBitCount;
        didOutput = true;
    } else if (m_outBitCount < m_mcs.getCodeWordLength()) {
        m_fifo.pop_front(); //drop the bits to be punctured
        ++m_outBitCount;
    }
    if (m_outBitCount == m_mcs.getCodeWordLength()) {
        m_outBitCount = 0;
        if (++m_outCodeWordCount == m_mcs.getNumCodeWords()) {
            m_outCodeWordCount = 0;
        }
    }

    return didOutput;
}

bool Puncture::handleRepetition(filter_io_t &data)
{
    size_t numRepeatBits = std::abs(m_mcs.getNumPuncOrRepBits());
    size_t numRepeatBitsPerSymbol = numRepeatBits / m_mcs.getNumOFDMSymbols();
    bool   didOutput = false;
    if (m_outCodeWordCount < (numRepeatBits % m_mcs.getNumOFDMSymbols())) {
        numRepeatBitsPerSymbol += 1; //the first rem(numRepeatBits, numOFDMSymbols) get an extra repeat bit
    }
    if (m_outBitCount < m_mcs.getCodeWordLength()) {
        if (m_fifo.empty()) {
            return false;
        }
        //we need to buffer the first numRepeatBits of the codeword for repetition
        if (m_outBitCount < numRepeatBitsPerSymbol) {
            m_buffer[m_outBitCount] = m_fifo.front();
        }
        //simply pass the codeword through
        data.type = IO_TYPE_BIT;
        data.bit = m_fifo.front();
        m_fifo.pop_front();
        ++m_outBitCount;
        didOutput = true;
    } else if (m_outBitCount < m_mcs.getCodeWordLength() + numRepeatBitsPerSymbol) {
        //now pass through the repeat bits that we just buffered
        data.type = IO_TYPE_BIT;
        data.bit = m_buffer[m_outBitCount - m_mcs.getCodeWordLength()];
        ++m_outBitCount;
        didOutput = true;
    }
    if (m_outBitCount == (m_mcs.getCodeWordLength() + numRepeatBitsPerSymbol)) {
        m_outBitCount = 0; //we're done repeating this codeword
        if (++m_outCodeWordCount == m_mcs.getNumCodeWords()) {
            m_outCodeWordCount = 0;
        }
    }
    return didOutput;
}

bool Puncture::output(filter_io_t &data)
{
    bool didOutput = false;
    if (m_doPuncture) {
        didOutput = handlePuncture(data);
    } else { //repeating bits
        didOutput = handleRepetition(data);
    }
    return didOutput;
}

void Puncture::tick()
{

}


