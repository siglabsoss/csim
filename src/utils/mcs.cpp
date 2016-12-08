#include <utils/mcs.hpp>
#include <cassert>
#include <cmath>
#include <algorithm>

// std::vector<bool> MCS::generateOFDMFrames(const std::vector<bool> &data)
// {
//    /* Phase 1 - Calculate minimum number of OFDM symbols necessary */
//    size_t numBitsPerSymbol = getNumBitsPerSymbol(m_mod, m_numOFDMCarriers);
//    size_t numInfoBits = data.size();
//    size_t numSymbols = getNumOFDMSymbols(numInfoBits, numBitsPerSymbol, m_rate);
//    size_t numTotalCodedBits = numBitsPerSymbol * numSymbols;

//    /* Phase 2 - Determine LDPC code word size & number of code words necessary */
//    size_t numCodeWords = getNumCodeWords(m_rate, numTotalCodedBits, numInfoBits);
//    size_t codeWordLength = getCodeWordLength(m_rate, numTotalCodedBits, numInfoBits);
//    size_t numInfoBitsPerCodeWord = codeWordLength * getCodeRateAsDouble(m_rate);

//    std::vector<bool> result(numCodeWords * numInfoBitsPerCodeWord);

//    /* Phase 3 - Determine the number of shortening bits necessary */
//    size_t numShortBits = getNumShorteningBits(numCodeWords, codeWordLength, m_rate, numInfoBits);
//    size_t numShortBitsPerCodeWord = static_cast<size_t>(floor(numShortBits / numCodeWords));
//    size_t numExtraShortBits = numShortBits % numCodeWords;

//    size_t dataIdx = 0;
//    for (size_t i = 0; i < numCodeWords; ++i) {
//         size_t shortBits = numShortBitsPerCodeWord;
//         if (i < numExtraShortBits) {
//             ++shortBits; //the first rem(numShortBits, numCodeWords) will have one more shortening bit
//         }
//         for (size_t j = 0; j < numInfoBitsPerCodeWord - shortBits; j++) {
//             result[ i * numCodeWords + j ] = data[dataIdx++];
//         }
//         for (size_t j = numInfoBitsPerCodeWord - shortBits; j < numInfoBitsPerCodeWord; j++) {
//             result[ i * numCodeWords + j ] = 0; //stuff shortening bits
//         }
//    }
//    assert(dataIdx == numInfoBits); //we should have copied everything over

//    /* Phase 4 - Determine the number of puncture (or repetition) bits necessary */
//    // ssize_t puncOrRepBits = getNumPuncOrRepBits(numInfoBits, numTotalCodedBits, numCodeWords, codeWordLength, m_rate);

//    return result;
// }

MCS::MCS(MCS::code_rate_t rate, MCS::modulation_t mod, size_t frame, size_t subs) :
  m_rate(rate),
  m_mod(mod),
  m_frame(frame),
  m_subs(subs)
{
}

size_t MCS::getNumBitsPerSubcarrier() const
{
    switch(m_mod) {
        case MOD_BPSK:
            return 1;
        case MOD_QPSK:
            return 2;
        case MOD_8PSK:
            return 3;
        case MOD_QAM16:
            return 4;
        case MOD_QAM64:
            return 6;
        case MOD_NULL:
        default:
            return 0;
    }
}

size_t MCS::getNumBitsPerSymbol() const
{
    size_t numBitsPerCarrier = MCS::getNumBitsPerSubcarrier();
    return numBitsPerCarrier * m_subs;
}

double MCS::getCodeRateAsDouble() const
{
    switch(m_rate) {
        case ONE_HALF_RATE:
            return 1.0/2.0;
        case TWO_THIRDS_RATE:
            return 2.0/3.0;
        case THREE_FOURTHS_RATE:
            return 3.0/4.0;
        case FIVE_SIXTHS_RATE:
            return 5.0/6.0;
        default:
            return 0.0;
    }
}

size_t MCS::getNumOFDMSymbols() const
{
    size_t numBitsPerSymbol = getNumBitsPerSymbol();
    double numDataBitsPerSymbol = getCodeRateAsDouble() * numBitsPerSymbol;
    return static_cast<size_t>(ceil(m_frame / numDataBitsPerSymbol));
}

size_t MCS::getNumCodeWords() const
{
    size_t totalCodedBits = getNumBitsPerSymbol() * getNumOFDMSymbols();
    if (totalCodedBits <= 1944) {
        return 1;
    } else if (totalCodedBits <= 2592) {
        return 2;
    } else {
        double rate = getCodeRateAsDouble();
        return static_cast<size_t>(ceil(static_cast<double>(m_frame) / (rate * 1944)));
    }
}

size_t MCS::getCodeWordLength() const
{
    size_t totalCodedBits = getNumBitsPerSymbol() * getNumOFDMSymbols();
    double parityRate = 1.0 - getCodeRateAsDouble();
    if (totalCodedBits <= 648) {
        if (totalCodedBits >= m_frame + 912 * parityRate) {
            return 1296;
        } else {
            return 648;
        }
    } else if (totalCodedBits <= 1296) {
        if (totalCodedBits >= m_frame + 1464 * parityRate) {
            return 1944;
        } else {
            return 1296;
        }
    } else if (totalCodedBits <= 1944) {
        return 1944;
    } else if (totalCodedBits <= 2592) {
        if (totalCodedBits >= m_frame + 2916 * parityRate) {
            return 1944;
        } else {
            return 1296;
        }
    } else {
        return 1944;
    }
}

size_t MCS::getNumShorteningBits() const
{
    size_t numCodeWords = getNumCodeWords();
    size_t codeWordLength = getCodeWordLength();
    size_t infoBitsAllotted = static_cast<size_t>((numCodeWords * codeWordLength) * getCodeRateAsDouble());
    return std::max(0, static_cast<int>(infoBitsAllotted - m_frame));
}

ssize_t MCS::getNumPuncOrRepBits() const
{
    size_t totalCodedBits = getNumBitsPerSymbol() * getNumOFDMSymbols();
    ssize_t numBitsNecessary = m_frame + getNumCodeWords() * getCodeWordLength() * (1.0 - getCodeRateAsDouble());
    return numBitsNecessary - static_cast<ssize_t>(totalCodedBits);
}

MCS::code_rate_t MCS::getCodingRate() const
{
    return m_rate;
}

MCS::modulation_t MCS::getModulation() const
{
  return m_mod;
}

size_t MCS::getFrameSize() const
{
    return m_frame;
}

size_t MCS::getNumSubCarriers() const
{
    return m_subs;
}

