#include <test/unit_test.hpp>

#include <utils/utils.hpp>

CSIM_TEST_SUITE_BEGIN(UtilitiesTests)

CSIM_TEST_CASE(HAMMING_DISTANCE)
{
    uint8_t a = 0b10101010;
    uint8_t b = 0b01010101;
    unsigned int dist = calculateHammingDistance(a, b);
    BOOST_CHECK_EQUAL(dist, 8);

    a = 0b10101010;
    b = 0b10101010;
    dist = calculateHammingDistance(a, b);
    BOOST_CHECK_EQUAL(dist, 0);

    a = 0b10010110;
    b = 0b10101010;
    dist = calculateHammingDistance(a, b);
    BOOST_CHECK_EQUAL(dist, 4);
}


CSIM_TEST_CASE(DYNAMIC_RANGE_SHIFT_CALCULATION)
{
    int shiftBits = utils::getShiftAmount(0.00582390);
    BOOST_CHECK_EQUAL(shiftBits, 7);

    shiftBits = utils::getShiftAmount(-0.00582390);
    BOOST_CHECK_EQUAL(shiftBits, 7);

    shiftBits = utils::getShiftAmount(0.003322259);
    BOOST_CHECK_EQUAL(shiftBits, 8);

    shiftBits = utils::getShiftAmount(0.5);
    BOOST_CHECK_EQUAL(shiftBits, 1);

    shiftBits = utils::getShiftAmount(0.999999);
    BOOST_CHECK_EQUAL(shiftBits, 0);

    shiftBits = utils::getShiftAmount(1.0);
    BOOST_CHECK_EQUAL(shiftBits, 0);

    shiftBits = utils::getShiftAmount(2.5);
    BOOST_CHECK_EQUAL(shiftBits, 0);

    shiftBits = utils::getShiftAmount(150);
    BOOST_CHECK_EQUAL(shiftBits, 0);
}

CSIM_TEST_CASE(DYNAMIC_RANGE_INTEGER_WIDTH_CALCULATION)
{
    unsigned width = utils::getIntegerBits(32767.5);
    BOOST_CHECK_EQUAL(width, 16); //15 integer + 1 sign

    width = utils::getIntegerBits(4567);
    BOOST_CHECK_EQUAL(width, 14); //13 integer + 1 sign

    width = utils::getIntegerBits(-4567);
    BOOST_CHECK_EQUAL(width, 14); //13 integer + 1 sign

    width = utils::getIntegerBits(0.4567);
    BOOST_CHECK_EQUAL(width, 1); //0 integer + 1 sign
}

CSIM_TEST_CASE(ADDITION32_OVERFLOW)
{
    BOOST_CHECK(utils::addition32DoesOverflow(-1, -1) == false);
    BOOST_CHECK(utils::addition32DoesOverflow(0b01111111111111111111111111111111, -1) == false);
    BOOST_CHECK(utils::addition32DoesOverflow(0b01111111111111111111111111111111,  1) == true);

    BOOST_CHECK(utils::addition32DoesOverflow(0b01111111111111111111111111111000,  8) == true);

    BOOST_CHECK(utils::addition32DoesOverflow(0b10000000000000000000000000000000,  1) == false);
    BOOST_CHECK(utils::addition32DoesOverflow(0b10000000000000000000000000000000, -1) == true);
}


CSIM_TEST_SUITE_END()
