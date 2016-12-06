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

double MCS::getCodeRateAsDouble(code_rate_t rate)
{
    switch(rate) {
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

size_t MCS::getNumBitsPerSymbol(MCS::modulation_t modulation, size_t numOFDMCarriers)
{
    size_t numBitsPerCarrier = MCS::getBitsPerSymbol(modulation);
    return numBitsPerCarrier * numOFDMCarriers;
}

size_t MCS::getNumOFDMSymbols(size_t numInfoBits, size_t numBitsPerSymbol, code_rate_t rate)
{
    double numDataBitsPerSymbol = getCodeRateAsDouble(rate) * numBitsPerSymbol;
    return static_cast<size_t>(ceil(numInfoBits / numDataBitsPerSymbol));
}

size_t MCS::getNumCodeWords(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits)
{
    if (totalCodedBits <= 1944) {
        return 1;
    } else if (totalCodedBits <= 2592) {
        return 2;
    } else {
        double rate_d = getCodeRateAsDouble(rate);
        return static_cast<size_t>(ceil(static_cast<double>(numInfoBits) / (rate_d * 1944)));
    }
}

size_t MCS::getCodeWordLength(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits)
{
    double parityRate = 1.0 - getCodeRateAsDouble(rate);
    if (totalCodedBits <= 648) {
        if (totalCodedBits >= numInfoBits + 912 * parityRate) {
            return 1296;
        } else {
            return 648;
        }
    } else if (totalCodedBits <= 1296) {
        if (totalCodedBits >= numInfoBits + 1464 * parityRate) {
            return 1944;
        } else {
            return 1296;
        }
    } else if (totalCodedBits <= 1944) {
        return 1944;
    } else if (totalCodedBits <= 2592) {
        if (totalCodedBits >= numInfoBits + 2916 * parityRate) {
            return 1944;
        } else {
            return 1296;
        }
    } else {
        return 1944;
    }
}

size_t MCS::getNumShorteningBits(size_t numCodeWords, size_t codeWordLength, code_rate_t rate, size_t numInfoBits)
{
    size_t infoBitsAllotted = static_cast<size_t>((numCodeWords * codeWordLength) * getCodeRateAsDouble(rate));
    return std::max(0, static_cast<int>(infoBitsAllotted - numInfoBits));
}

ssize_t MCS::getNumPuncOrRepBits(size_t numInfoBits, size_t totalCodedBits, size_t numCodeWords, size_t codeWordLength, code_rate_t rate)
{
    ssize_t numBitsNecessary = numInfoBits + numCodeWords * codeWordLength * (1.0 - getCodeRateAsDouble(rate));
    return numBitsNecessary - static_cast<ssize_t>(totalCodedBits);
}


size_t MCS::getBitsPerSymbol(MCS::modulation_t mod)
{
    switch(mod) {
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