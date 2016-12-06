#pragma once

#include <filters/mapper.hpp>
#include <cmath>

class FrameGen
{
public:
    enum code_rate_t {
        ONE_HALF_RATE = 0,
        TWO_THIRDS_RATE,
        THREE_FOURTHS_RATE,
        FIVE_SIXTHS_RATE
    };
    FrameGen(code_rate_t rate, Mapper::constellation_set_t modulation, size_t numOFDMCarriers);
    std::vector<bool> generateOFDMFrames(const std::vector<uint8_t> &data);
private:
    static double getCodeRateAsDouble(code_rate_t rate);
    static size_t getNumBitsPerSymbol(Mapper::constellation_set_t modulation, size_t numOFDMCarriers);
    static size_t getNumOFDMSymbols(size_t numInfoBits, size_t numBitsPerSymbol, code_rate_t rate);
    static size_t getNumCodeWords(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits);
    static size_t getCodeWordLength(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits);
    static size_t getNumShorteningBits(size_t numCodeWords, size_t codeWordLength, code_rate_t rate, size_t numInfoBits);
    static ssize_t getNumPuncOrRepBits(size_t numInfoBits, size_t totalCodedBits, size_t numCodeWords, size_t codeWordLength, code_rate_t rate);

    Mapper::constellation_set_t m_mod;
    code_rate_t m_rate;
    size_t m_numOFDMCarriers;
};
