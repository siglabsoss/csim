#pragma once

#include <stdlib.h>

namespace MCS
{
    enum code_rate_t {
        ONE_HALF_RATE = 0,
        TWO_THIRDS_RATE,
        THREE_FOURTHS_RATE,
        FIVE_SIXTHS_RATE
    };
    enum modulation_t {
        MOD_NULL = 0,
        MOD_BPSK,
        MOD_QPSK,
        MOD_8PSK,
        MOD_QAM16,
        MOD_QAM64
    };
    // std::vector<bool> generateOFDMFrames(const std::vector<bool> &data);
    size_t     getBitsPerSymbol(MCS::modulation_t mod);

    double getCodeRateAsDouble(code_rate_t rate);
    size_t getNumBitsPerSymbol(MCS::modulation_t modulation, size_t numOFDMCarriers);
    size_t getNumOFDMSymbols(size_t numInfoBits, size_t numBitsPerSymbol, code_rate_t rate);
    size_t getNumCodeWords(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits);
    size_t getCodeWordLength(code_rate_t rate, size_t totalCodedBits, size_t numInfoBits);
    size_t getNumShorteningBits(size_t numCodeWords, size_t codeWordLength, code_rate_t rate, size_t numInfoBits);
    ssize_t getNumPuncOrRepBits(size_t numInfoBits, size_t totalCodedBits, size_t numCodeWords, size_t codeWordLength, code_rate_t rate);

};
