#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE Radio

#include <core/radio_s.hpp>


#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(FILTER_CHAIN_IS_PROPERLY_CHAINED)
{
    radio_config_t config;
    config.position = Vector2d(0,0);
    RadioS *radio = RadioS::create(config);

    uint8_t byte = 0;

    //Receive an RF sample, which in turn feeds through the demodulation filter chain
    for (int i = 0; i < 11; i++) {
        std::complex<double> sample(0.5, 0.5);
        radio->rxWave(sample);
        bool didOutput = radio->rxByte(byte);
        BOOST_CHECK_EQUAL(didOutput, (i == 10));
    }

    BOOST_CHECK_EQUAL(byte, 0xaa);

    //Transmit a byte, which in turn feeds through the modulation filter chain
    {
        std::complex<double> sample;
        byte = 0xAA;
        radio->txByte(byte);
        bool didOutput = radio->txWave(sample);
        BOOST_CHECK(didOutput == true);
        BOOST_CHECK_CLOSE(sample.real(), 346.0, DBL_EPSILON);
        BOOST_CHECK_CLOSE(sample.imag(), 0.0, DBL_EPSILON);
    }
}
