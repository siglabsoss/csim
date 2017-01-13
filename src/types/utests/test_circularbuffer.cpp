#include <test/unit_test.hpp>
#include <types/circularbuffer.hpp>

CSIM_TEST_SUITE_BEGIN(CIRCULAR_BUFFER_INDEXING)

CSIM_TEST_CASE(COMPLEX_CONJUGATE)
{
    constexpr size_t BUFFER_CAPACITY = 10;

    CircularBuffer<int> buf(BUFFER_CAPACITY);
        BOOST_CHECK_EQUAL(buf.size(),     0);
        BOOST_CHECK_EQUAL(buf.capacity(), BUFFER_CAPACITY);

    for (size_t i = 0; i < BUFFER_CAPACITY / 2; ++i) {
        buf.push_back(i);
    }
        BOOST_CHECK_EQUAL(buf.size(), 5);

    for (size_t i = 0; i < BUFFER_CAPACITY / 2; ++i) {
        BOOST_CHECK_EQUAL(buf[i],     i);
    }
}

CSIM_TEST_CASE(CIRCULAR_BUFFER_FULL_IDX_ROTATION)
{
    constexpr size_t BUFFER_CAPACITY = 10;

    CircularBuffer<int> buf(BUFFER_CAPACITY);

    for (size_t i = 0; i < BUFFER_CAPACITY; ++i) {
        buf.push_back(i);
    }
    buf.push_back(10);

    size_t tenIdx = buf.size() - 1;

    for (size_t i = 0; i < 5; ++i) {
        buf.push_back(11 + i);
        tenIdx--;
    }
    BOOST_CHECK_EQUAL(buf[tenIdx], 10);
}

CSIM_TEST_SUITE_END()
