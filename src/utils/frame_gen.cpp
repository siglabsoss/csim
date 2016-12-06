#include <utils/frame_gen.hpp>

FrameGen::FrameGen(code_rate_t rate, Mapper::constellation_set_t modulation, size_t numOFDMCarriers) :
        m_mod(modulation),
        m_rate(rate),
        m_numOFDMCarriers(numOFDMCarriers)
{
}
//
//std::vector<bool> FrameGen::generateOFDMFrames(const std::vector<uint8_t> &data)
//{
//    /* Phase 1 - Calculate minimum number of OFDM symbols necessary */
//    size_t numBitsPerSymbol = getNumBitsPerSymbol(m_mod, m_numOFDMCarriers);
//    size_t numInfoBits = data.size() * 8;
//    size_t numSymbols = getNumOFDMSymbols(numInfoBits, numBitsPerSymbol, m_rate);
//    size_t numTotalCodedBits = numBitsPerSymbol * numSymbols;
//
//    /* Phase 2 - Determine code word size & number of code words necessary */
//    size_t numCodeWords = getNumCodeWords(m_rate, numTotalCodedBits, numInfoBits);
//    size_t codeWordLength = getCodeWordLength(m_rate, numTotalCodedBits, numInfoBits);
//
//    /* Phase 3 - Determine the number of shortening bits necessary */
//    size_t numShortBits = getNumShorteningBits(numCodeWords, codeWordLength, m_rate, numInfoBits);
//
//    /* Phase 4 - Determine the number of puncture (or repetition) bits necessary */
//    ssize_t puncOrRepBits = getNumPuncOrRepBits(numInfoBits, numTotalCodedBits, numCodeWords, codeWordLength, m_rate);
//
//    return std::vector<bool>(); //XXX implement
//}

double FrameGen::getCodeRateAsDouble(code_rate_t rate)
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

size_t FrameGen::getNumBitsPerSymbol(Mapper::constellation_set_t modulation, size_t numOFDMCarriers)
{
    size_t numBitsPerCarrier = Mapper::getBitsPerSymbol(modulation);
    return numBitsPerCarrier * numOFDMCarriers;
}

size_t FrameGen::getNumOFDMSymbols(size_t numInfoBits, size_t numBitsPerSymbol, code_rate_t rate)
{
    double numDataBitsPerSymbol = getCodeRateAsDouble(rate) * numBitsPerSymbol;
    return static_cast<size_t>(ceil(numInfoBits / numDataBitsPerSymbol));
}

size_t FrameGen::getNumCodeWords(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits)
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

size_t FrameGen::getCodeWordLength(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits)
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

size_t FrameGen::getNumShorteningBits(size_t numCodeWords, size_t codeWordLength, code_rate_t rate, size_t numInfoBits)
{
    size_t infoBitsAllotted = static_cast<size_t>((numCodeWords * codeWordLength) * getCodeRateAsDouble(rate));
    return std::max(0, static_cast<int>(infoBitsAllotted - numInfoBits));
}

ssize_t FrameGen::getNumPuncOrRepBits(size_t numInfoBits, size_t totalCodedBits, size_t numCodeWords, size_t codeWordLength, code_rate_t rate)
{
    ssize_t numBitsNecessary = numInfoBits + numCodeWords * codeWordLength * (1.0 - getCodeRateAsDouble(rate));
    return numBitsNecessary - static_cast<ssize_t>(totalCodedBits);
}
