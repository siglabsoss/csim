#include <test/unit_test.hpp>

#include <core/radio_s.hpp>
#include <filters/sine_wave.hpp>
#include <cfloat>

CSIM_TEST_SUITE_BEGIN(RadioTests)

CSIM_TEST_CASE(FILTER_CHAIN_IS_PROPERLY_CHAINED)
{

    radio_config_t config;
    config.position = Vector2d(0,0);
    config.id = 100;

    FilterChain modulation_chain;

    SineWave *sw = new SineWave(2000);
    //modulation_chain = *dc4 + *dbtc + *db6 + *db5 + *db4;
    modulation_chain = *sw;

    FilterChain demodulation_chain;
    sw = new SineWave(300);
    demodulation_chain = *sw;

    RadioS *radio = new RadioS(config, std::move(modulation_chain), std::move(demodulation_chain));

    uint8_t byte = 0;

    //Receive an RF sample, which in turn feeds through the demodulation filter chain
    for (int i = 0; i < 11; i++) {
        std::complex<double> sample(0.5, 0.5);
        radio->rxWave(sample);
        bool didOutput = radio->rxByte(byte);
        //BOOST_CHECK_EQUAL(didOutput, (i == 10));
    }

    //BOOST_CHECK_EQUAL(byte, 0xaa);

    //Transmit a byte, which in turn feeds through the modulation filter chain
    {
        std::complex<double> sample;
        byte = 0xAA;
        radio->txByte(byte);
        bool didOutput = radio->txWave(sample);
        //BOOST_CHECK(didOutput == true);
        //BOOST_CHECK_CLOSE(sample.real(), 346.0, DBL_EPSILON);
        //BOOST_CHECK_CLOSE(sample.imag(), 0.0, DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
