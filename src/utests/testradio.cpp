#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Radio

#include "radio_s.hpp"
#include "dummy_filter_element.hpp"
#include "fixedfir.h"

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(FILTER_CHAIN_IS_PROPERLY_CHAINED)
{
    //Construct a radio with some dummy filter chains
    //FilterChain *mod = new FilterChain(new DummyByte(new DummyByte(new DummyByte(new DummyByteToComplex(new DummyComplex(nullptr))))));
    //FilterChain *demod = new FilterChain(new DummyComplex(new DummyComplex(new DummyComplex(new DummyComplexToByte(new DummyByte(new DummyByte(new DummyByte(nullptr))))))));
    FilterChain mod;
    DummyByte db4;
    DummyByte db5;
    DummyByte db6;
    DummyByteToComplex dbtc;
    DummyComplex dc4;

    mod = dc4 + dbtc + db6 + db5 + db4;

    FilterChain demod;
    DummyComplex dc1;
    DummyComplex dc2;
    DummyComplex dc3;
    DummyComplexToByte dctb;
    DummyByte db1;
    DummyByte db2;
    DummyByte db3;


    demod = db3 + db2 + db1 + dctb + dc3 + dc2 + dc1;
    RadioS radio(Vector2d(0,0), mod, demod);

    uint8_t byte;

    //Receive an RF sample, which in turn feeds through the demodulation filter chain
    std::complex<double> sample(1.123456, 2.0);
    radio.rxWave(sample);
    bool didOutput = radio.rxByte(byte);
    BOOST_CHECK_EQUAL(didOutput, true);
    BOOST_CHECK_EQUAL(byte, 0x1b);

    //Transmit a byte, which in turn feeds through the modulation filter chain
    byte = 0xAA;
    radio.txByte(byte);
    didOutput = radio.txWave(sample);
    BOOST_CHECK(didOutput == true);
    BOOST_CHECK_CLOSE(sample.real(), 346.0, DBL_EPSILON);
    BOOST_CHECK_CLOSE(sample.imag(), 0.0, DBL_EPSILON);
}
