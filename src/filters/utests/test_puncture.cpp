#include <test/unit_test.hpp>

#include <filters/puncture.hpp>

CSIM_TEST_SUITE_BEGIN(PuncturingAndRepetition)

CSIM_TEST_CASE(PROPER_REPETITION)
{
    size_t frameSize = (1944 / 2)*4;
    size_t numInputCodeWords = 10;
    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, frameSize, 1024);
    BOOST_CHECK(mcs.getNumPuncOrRepBits() < 0); //double check that our MCS calls for repetition
    size_t numRepeatBits = std::abs(mcs.getNumPuncOrRepBits());
    size_t numRepeatBitsPerSymbol = numRepeatBits / mcs.getNumOFDMSymbols();
    Puncture punc(mcs);
    std::vector<bool> inputs(mcs.getCodeWordLength() * numInputCodeWords);
    for (size_t i = 0; i < numInputCodeWords; ++i) {
        for (size_t j = 0; j < mcs.getCodeWordLength(); ++j) {
            if (j < numRepeatBitsPerSymbol) {
                inputs[i*mcs.getCodeWordLength() + j] = ((j % 2) == 0);
            } else {
                inputs[i*mcs.getCodeWordLength() + j] = !((j % 2) == 0);
            }
        }
    }
    filter_io_t data;
    for (size_t i = 0; i < numInputCodeWords; ++i) {
        for (size_t j = 0; j < mcs.getCodeWordLength(); ++j) {
            data.type = IO_TYPE_BIT;
            data.bit = inputs[i*mcs.getCodeWordLength() + j];
            punc.input(data);
            punc.tick();

            BOOST_CHECK(punc.output(data) == true);
            BOOST_REQUIRE(data.bit == inputs[i*mcs.getCodeWordLength() + j]);
        }
        for (size_t j = 0; j < numRepeatBitsPerSymbol; ++j) {
            punc.tick();
            BOOST_CHECK(punc.output(data) == true);
            BOOST_CHECK(data.bit == inputs[i*mcs.getCodeWordLength() + j]);
        }
    }
    punc.tick();
    BOOST_CHECK(punc.output(data) == false);
}

CSIM_TEST_SUITE_END()
