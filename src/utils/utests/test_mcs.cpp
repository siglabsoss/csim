#include <test/unit_test.hpp>

#define private public
#include <utils/mcs.hpp>
#undef private

CSIM_TEST_SUITE_BEGIN(MCSeration)

/*
 * Many of these test cases were borrowed from the examples in Chapter 6
 *(section 6.4.1) of
 * "Next Generation Wireless LANs: Throughput, Robustness, and Reliability in
 *802.11n"
 */

CSIM_TEST_CASE(MIN_OFDM_SYMBOLS_CALCULATION)
{
    constexpr size_t length      = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    MCS mcs(MCS::FIVE_SIXTHS_RATE,
            MCS::MOD_QAM64,
            numInfoBits,
            104,
            std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getNumOFDMSymbols(), 47);
}

CSIM_TEST_CASE(NUM_BITS_PER_SYMBOL)
{
    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_BPSK, 0, 104, std::vector<bool>(104, true));
    MCS mcs2(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM64, 0, 104, std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getNumBitsPerSymbol(),  104);
    BOOST_CHECK_EQUAL(mcs2.getNumBitsPerSymbol(), 6 * 104);
}

CSIM_TEST_CASE(NUM_CODE_WORDS)
{
    constexpr size_t length      = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    MCS mcs(MCS::FIVE_SIXTHS_RATE,
            MCS::MOD_QAM64,
            numInfoBits,
            104,
            std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getNumCodeWords(), 15);

    constexpr size_t length2      = 3053;
    constexpr size_t numInfoBits2 = 8 * length2 + 16;
    MCS mcs2(MCS::FIVE_SIXTHS_RATE,
             MCS::MOD_QAM64,
             numInfoBits2,
             104,
             std::vector<bool>(104, true));
    BOOST_CHECK_EQUAL(mcs2.getNumCodeWords(), 16);
}

CSIM_TEST_CASE(CODE_WORD_LENGTH)
{
    constexpr size_t length      = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    MCS mcs(MCS::FIVE_SIXTHS_RATE,
            MCS::MOD_QAM64,
            numInfoBits,
            104,
            std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getCodeWordLength(), 1944);
}

CSIM_TEST_CASE(NUM_SHORTENING_BITS)
{
    constexpr size_t length      = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    MCS mcs(MCS::FIVE_SIXTHS_RATE,
            MCS::MOD_QAM64,
            numInfoBits,
            104,
            std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getNumShorteningBits(), 284);
}

CSIM_TEST_CASE(NUM_REPETITION_BITS)
{
    constexpr size_t length      = 3000;
    constexpr size_t numInfoBits = 8 * length + 16;
    MCS mcs(MCS::FIVE_SIXTHS_RATE,
            MCS::MOD_QAM64,
            numInfoBits,
            104,
            std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getNumPuncOrRepBits(), -452);
}

CSIM_TEST_CASE(NUM_PUNCTURE_BITS)
{
    constexpr size_t length      = 3053;
    constexpr size_t numInfoBits = 8 * length + 16;
    MCS mcs(MCS::FIVE_SIXTHS_RATE,
            MCS::MOD_QAM64,
            numInfoBits,
            104,
            std::vector<bool>(104, true));

    BOOST_CHECK_EQUAL(mcs.getNumPuncOrRepBits(), 296);
}

// CSIM_TEST_CASE(CODEWORD_PREPARATION)
// {
//     constexpr size_t numInfoBits = 3240;
//     std::vector<bool> data(numInfoBits, 1);
//     MCS gen(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM64, 52);
//     std::vector<bool> result = gen.generateOFDMFrames(data);
// }

CSIM_TEST_SUITE_END()
