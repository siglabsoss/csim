#include <test/unit_test.hpp>
#include <types/fixedpoint.hpp>


CSIM_TEST_SUITE_BEGIN(SLFixedPointTests)

CSIM_TEST_CASE(BIT_REPRESENTATION)
{
    std::vector <std::pair <double, int64_t> > tests;

    tests.push_back( std::pair<double, int64_t> ( 00.0000, 0b00000000)); //0 000 0000
    tests.push_back( std::pair<double, int64_t> ( 01.0000, 0b00010000)); //0 001 0000
    tests.push_back( std::pair<double, int64_t> (-01.0000, 0b11110000)); //1 111 0000
    tests.push_back( std::pair<double, int64_t> (-01.5000, 0b11101000)); //1 110 1000
    tests.push_back( std::pair<double, int64_t> (-02.0000, 0b11100000)); //1 110 0000
    tests.push_back( std::pair<double, int64_t> (-02.5000, 0b11011000)); //1 101 1000 (why does the integer part change from -2.0 to -2.5?)
    tests.push_back( std::pair<double, int64_t> ( 01.5000, 0b00011000)); //0 001 1000
    tests.push_back( std::pair<double, int64_t> ( 01.7500, 0b00011100)); //0 001 1100
    tests.push_back( std::pair<double, int64_t> ( 01.8750, 0b00011110)); //0 001 1110
    tests.push_back( std::pair<double, int64_t> ( 01.9375, 0b00011111)); //0 001 1111
    tests.push_back( std::pair<double, int64_t> ( 01.9999, 0b00011111)); //0 001 1111 (truncation of precision)
    tests.push_back( std::pair<double, int64_t> ( 15.9999, 0b11111111)); //1 111 1111 (truncation of precision & roll over onto sign bit)
    tests.push_back( std::pair<double, int64_t> ( 16.0000, 0b00000000)); //0 001 0000 (roll over of integer component)
    tests.push_back( std::pair<double, int64_t> ( 17.0000, 0b00010000)); //0 001 0000 (roll over of integer component)
    tests.push_back( std::pair<double, int64_t> ( 17.9999, 0b00011111)); //0 001 0000 (roll over of integer component)

    SLFixedPoint<8, 4> fp;
    for (auto it = tests.begin(); it != tests.end(); it++) {
        fp = it->first;
        uint64_t result = fp.to_uint64();
        int64_t expectedResult = it->second;
        BOOST_CHECK_EQUAL(result, expectedResult);
    }
}

CSIM_TEST_CASE(BIT_REPRESENTATION_ALL_FRACTIONAL)
{
    //Note that 1 and -1 have the same underlying representation
    std::vector <std::pair <double, int64_t> > tests;

    tests.push_back( std::pair<double, int64_t> ( 00.0000000, 0b00000000));
    tests.push_back( std::pair<double, int64_t> ( 00.5000000, 0b01000000));
    tests.push_back( std::pair<double, int64_t> ( 00.7500000, 0b01100000));
    tests.push_back( std::pair<double, int64_t> ( 00.8750000, 0b01110000));
    tests.push_back( std::pair<double, int64_t> ( 00.9375000, 0b01111000));
    tests.push_back( std::pair<double, int64_t> ( 00.9687500, 0b01111100));
    tests.push_back( std::pair<double, int64_t> ( 00.9843750, 0b01111110));
    tests.push_back( std::pair<double, int64_t> ( 00.9921875, 0b01111111));
    tests.push_back( std::pair<double, int64_t> ( 01.0000000, 0b10000000)); //overflow to sign bit
    tests.push_back( std::pair<double, int64_t> (-00.5000000, 0b11000000));
    tests.push_back( std::pair<double, int64_t> (-00.7500000, 0b10100000));
    tests.push_back( std::pair<double, int64_t> (-00.8750000, 0b10010000));
    tests.push_back( std::pair<double, int64_t> (-00.9375000, 0b10001000));
    tests.push_back( std::pair<double, int64_t> (-00.9687500, 0b10000100));
    tests.push_back( std::pair<double, int64_t> (-00.9843750, 0b10000010));
    tests.push_back( std::pair<double, int64_t> (-00.9921875, 0b10000001));
    tests.push_back( std::pair<double, int64_t> (-01.0000000, 0b10000000)); //underflow -1.0 = 1.0
    tests.push_back( std::pair<double, int64_t> ( 01.5000000, 0b11000000));
    tests.push_back( std::pair<double, int64_t> (-01.5000000, 0b01000000)); //underflow -1.5 = 0.5

    SLFixedPoint<8, 1> fp;
    for (auto it = tests.begin(); it != tests.end(); it++) {
        fp = it->first;
        int64_t result = fp.to_uint64();
        int64_t expectedResult = it->second;
        BOOST_CHECK_EQUAL(result, expectedResult);
    }
}

CSIM_TEST_CASE(ALL_FRACTIONAL_ADDITION)
{
    std::vector <std::pair <double, int64_t> > tests;

    SLFixedPoint<8, 1> a;
    SLFixedPoint<8, 1> b;
    SLFixedPoint<8, 1> c;
    a = 0.5;
    b = 0.5;
    c = a + b;

    int64_t result = c.to_uint64();
    int64_t expectedResult = 0b10000000;
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 0.5; b = 0.5078125; c = a + b;
    result = c.to_uint64();
    expectedResult = 0b10000001; //1000 0001 = 0100 0000 + 0100 0001
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 1.0; b = 1.0; c = a + b;
    result = c.to_uint64();
    expectedResult = 0b00000000; //0001 [0000 0000] = 1000 0000 + 1000 0000
    BOOST_CHECK_EQUAL(result, expectedResult);
}

CSIM_TEST_CASE(POSITIVE_MULTIPLICATION)
{
    SLFixedPoint<8, 4> a;
    SLFixedPoint<8, 4> b;
    SLFixedPoint<8, 4> c;
    SLFixedPoint<12, 8> d;
    SLFixedPoint<16, 8> e;
    a = 7.0; //0 111 0000
    b = 7.0; //0 111 0000
    c = a * b;
    int64_t result = c.to_uint64();
    int64_t expectedResult = 0b00010000; // 0011 [0001 0000] 0000 = 0111 0000 * 0111 0000
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 1.0; b = 1.0; c = a * b; // 1 = 0 001 0000
    result = c.to_uint64();
    expectedResult = 0b00010000; // [0001 0000] 0000
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 0.9999; b = 0.9999; c = a * b; // 0.9999 = 0 000 1111
    result = c.to_uint64();
    expectedResult = 0b00001110; // [0000 1110] 0001
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 1.9999; b = 1.9999; c = a * b; // 1.9999 = 0 001 1111
    result = c.to_uint64();
    expectedResult = 0b00111100; // [0011 1100] 0001
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 7.0; b = 7.0;
    d = a * b;
    result = d.to_uint64();
    expectedResult = 0b001100010000; // [0011 0001 0000] 0000
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 7.9999; b = 7.9999; // 0 111 1111
    d = a * b;
    result = d.to_uint64();
    expectedResult = 0b1111110000; // [0011 1111 0000] 0001
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 7.9999; b = 7.9999; // 0 111 1111
    e = a * b;
    result = e.to_uint64();
    expectedResult = 0b11111100000001; // [0011 1111 0000 0001]
    BOOST_CHECK_EQUAL(result, expectedResult);
}

CSIM_TEST_CASE(FRACTIONAL_MULTIPLICATION)
{
    SLFixedPoint<8, 1> a;
    SLFixedPoint<8, 1> b;
    SLFixedPoint<8, 1> c;
    a = 0.5; //0 1000000
    b = 0.5; //0 1000000
    c = a * b;
    int64_t result = c.to_uint64();
    int64_t expectedResult = 0b00100000; // 00[001 0000 0]000 0000
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 1.0; b = 1.0; c = a * b; // 1 0000000 (roll over to sign bit)
    result = c.to_uint64();
    expectedResult = 0b10000000; // 0[100 0000 0]000 0000
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 1.0; b = 1.5; c = a * b; // 1 0000000 * 1 1000000 (roll over to sign bit)
    result = c.to_uint64();
    expectedResult = 0b01000000; // 01[10 0000 0]000 0000
    BOOST_CHECK_EQUAL(result, expectedResult);

    a = 1.0; b = -0.9921875; c = a * b; // 1 0000000 * 1 0000001 (roll over to sign bit) (-0.9921875 == 1.0078125)
    result = c.to_uint64();
    expectedResult = 0b01111111; // 0[100 0000 1]000 0000
    BOOST_CHECK_EQUAL(result, expectedResult);
}

CSIM_TEST_CASE(DOUBLE_TO_FIXED_TO_DOUBLE)
{
    constexpr double testVal = 0.999999;
    SLFixPoint a;
    for (size_t width = 16; width < 64; width++) {
        a.setFormat(width, 1);
        a = testVal;
        BOOST_CHECK_CLOSE(a.to_double(), testVal, 0.005);
    }
}

CSIM_TEST_CASE(ADDITION_OF_DIFFERING_FORMATS)
{
    SLFixPoint a, b, c;
    a.setFormat(48, 2);
    b.setFormat(16, 1);
    c.setFormat(32, 1);
    a = 0.3;
    b = -0.3;
    c = a - b;
    BOOST_CHECK_CLOSE(c.to_double(), 0.6, 0.005);
}

CSIM_TEST_SUITE_END()
