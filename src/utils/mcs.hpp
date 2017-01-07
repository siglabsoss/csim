#pragma once

#include <cstdlib>
#include <vector>

class MCS
{
public:

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

    static constexpr size_t NUM_OFDM_CARRIERS = 1024;

    MCS(code_rate_t rate,
        modulation_t mod,
        size_t frame,
        size_t subs = NUM_OFDM_CARRIERS,
        std::vector<bool>subMask = std::vector<bool>(NUM_OFDM_CARRIERS, true));

    // std::vector<bool> generateOFDMFrames(const std::vector<bool> &data);
    size_t       getNumBitsPerSubcarrier() const;
    size_t       getNumBitsPerSymbol() const;
    double       getCodeRateAsDouble() const;
    size_t       getNumOFDMSymbols() const;
    size_t       getNumCodeWords() const;
    size_t       getCodeWordLength() const;
    size_t       getNumShorteningBits() const;
    ssize_t      getNumPuncOrRepBits() const;

    code_rate_t  getCodingRate() const;
    modulation_t getModulation() const;
    size_t       getFrameSize() const;
    size_t       getNumSubCarriers() const;
    size_t       getNumActiveSubCarriers() const;

private:

    code_rate_t m_rate;
    modulation_t m_mod;
    size_t m_frame;
    size_t m_subs;
    size_t m_activeSubs;
    std::vector<bool> m_subMask;
};
