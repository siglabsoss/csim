#include <test/unit_test.hpp>

#define private public
#include <utils/mcs.hpp>
#undef private

CSIM_TEST_SUITE_BEGIN(MCSeration)

/*
 * Many of these test cases were borrowed from the examples in Chapter 6 (section 6.4.1) of
 * "Next Generation Wireless LANs: Throughput, Robustness, and Reliability in 802.11n"
 */

CSIM_TEST_CASE(MIN_OFDM_SYMBOLS_CALCULATION)
{
    constexpr size_t length = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    BOOST_CHECK_EQUAL(MCS::getNumOFDMSymbols(numInfoBits, 1040, MCS::ONE_HALF_RATE), 47);
}

CSIM_TEST_CASE(NUM_BITS_PER_SYMBOL)
{
    BOOST_CHECK_EQUAL(MCS::getNumBitsPerSymbol(MCS::MOD_BPSK, 52), 52);
    BOOST_CHECK_EQUAL(MCS::getNumBitsPerSymbol(MCS::MOD_QAM16, 52), 208);
}

CSIM_TEST_CASE(NUM_CODE_WORDS)
{
    constexpr size_t length = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    BOOST_CHECK_EQUAL(MCS::getNumCodeWords(MCS::FIVE_SIXTHS_RATE, 29328, numInfoBits), 15);

    constexpr size_t length2 = 3053;
    constexpr size_t numInfoBits2 = 8 * length2 + 16;
    BOOST_CHECK_EQUAL(MCS::getNumCodeWords(MCS::FIVE_SIXTHS_RATE, 29328, numInfoBits2), 16);
}

CSIM_TEST_CASE(CODE_WORD_LENGTH)
{
    constexpr size_t length = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    BOOST_CHECK_EQUAL(MCS::getCodeWordLength(MCS::FIVE_SIXTHS_RATE, 29328, numInfoBits), 1944);
}

CSIM_TEST_CASE(NUM_SHORTENING_BITS)
{
    constexpr size_t length = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    constexpr size_t numCodeWords = 15;
    constexpr size_t codeWordLength = 1944;
    BOOST_CHECK_EQUAL(MCS::getNumShorteningBits(numCodeWords, codeWordLength, MCS::FIVE_SIXTHS_RATE, numInfoBits), 284);
}

CSIM_TEST_CASE(NUM_REPETITION_BITS)
{
    constexpr size_t length = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    constexpr size_t numCodeWords = 15;
    constexpr size_t codeWordLength = 1944;
    BOOST_CHECK_EQUAL(MCS::getNumPuncOrRepBits(numInfoBits, 29328, numCodeWords, codeWordLength, MCS::FIVE_SIXTHS_RATE), -452);
}

CSIM_TEST_CASE(NUM_PUNCTURE_BITS)
{
    constexpr size_t length = 3053;
    constexpr size_t numInfoBits = 8 * length + 16;
    constexpr size_t numCodeWords = 16;
    constexpr size_t codeWordLength = 1944;
    BOOST_CHECK_EQUAL(MCS::getNumPuncOrRepBits(numInfoBits, 29328, numCodeWords, codeWordLength, MCS::FIVE_SIXTHS_RATE), 296);
}

// CSIM_TEST_CASE(CODEWORD_PREPARATION)
// {
//     constexpr size_t numInfoBits = 3240;
//     std::vector<bool> data(numInfoBits, 1);
//     MCS gen(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM64, 52);
//     std::vector<bool> result = gen.generateOFDMFrames(data);
// }

CSIM_TEST_SUITE_END()
