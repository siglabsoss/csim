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

CSIM_TEST_SUITE_END()
