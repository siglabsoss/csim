#include <test/unit_test.hpp>

#include <filters/scrambler_block.hpp>

CSIM_TEST_SUITE_BEGIN(ScramblerValidation)

//This particular scrambler is based on an LFSR that's independent of the data and repeats the following 127-bit pattern that gets XOR'd with input data
//00001110 11110010 11001001 00000010 00100110 00101110 10110110 00001100 11010100 11100111 10110100 00101010 11111010 01010001 10111000 1111111
static bool pattern[127] = {0,0,0,0,1,1,1,0, 1,1,1,1,0,0,1,0, 1,1,0,0,1,0,0,1, 0,0,0,0,0,0,1,0, 0,0,1,0,0,1,1,0, 0,0,1,0,1,1,1,0, 1,0,1,1,0,1,1,0, 0,0,0,0,1,1,0,0, 1,1,0,1,0,1,0,0, 1,1,1,0,0,1,1,1, 1,0,1,1,0,1,0,0, 0,0,1,0,1,0,1,0, 1,1,1,1,1,0,1,0, 0,1,0,1,0,0,0,1, 1,0,1,1,1,0,0,0, 1,1,1,1,1,1,1};
CSIM_TEST_CASE(SCRAMBLER_BIT_SEQUENCE)
{
    std::vector<bool> dummyData(254, 0);
    for (size_t i = 127; i < 254; i++) {
        dummyData[i] = 1; //second half is all ones
    }

    ScramblerBlock s;
    filter_io_t sample;
    sample.type = IO_TYPE_BIT;
    for (size_t i = 0; i < dummyData.size(); i++) {
        sample.bit = dummyData[i];
        s.input(sample);
        s.tick();
        BOOST_CHECK(s.output(sample) == true);
        BOOST_CHECK_EQUAL(sample.type, IO_TYPE_BIT);
        //first half of dummy data is all 0's so scrambler should output the 127-bit pattern
        //second half of dummy data is all 1's so scrambler should output inverse of 127-bit pattern
        if (i < 127) {
            BOOST_CHECK_EQUAL(sample.bit, pattern[i]);
        } else {
            BOOST_CHECK_EQUAL(sample.bit, !pattern[i - 127]);
        }
    }
}

CSIM_TEST_CASE(SCRAMBLER_RESET)
{
    std::vector<bool> dummyData(10, 0);
    ScramblerBlock s;
    filter_io_t sample;
    sample.type = IO_TYPE_BIT;
    for (size_t i = 0; i < 10; i++) {
        sample.bit = dummyData[i];
        s.input(sample);
        s.tick();
        BOOST_CHECK(s.output(sample) == true);
        BOOST_CHECK_EQUAL(sample.bit, pattern[i]);
    }
    dummyData = std::vector<bool>(10, 0);
    for (size_t i = 0; i < 10; i++) {
        sample.bit = dummyData[i];
        s.input(sample);
        s.tick();
        BOOST_CHECK(s.output(sample) == true);
        //since state was not reset, dummyData should equal the pattern 10 bits in
        BOOST_CHECK_EQUAL(sample.bit, pattern[i+10]);
    }
    dummyData = std::vector<bool>(10, 0);
    s.reset(0b1111111);
    for (size_t i = 0; i < 10; i++) {
        sample.bit = dummyData[i];
        s.input(sample);
        s.tick();
        BOOST_CHECK(s.output(sample) == true);
        //since state was reset, dummyData should equal the pattern from the start
        BOOST_CHECK_EQUAL(sample.bit, pattern[i]);
    }
}


CSIM_TEST_SUITE_END()
