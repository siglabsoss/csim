#include <test/unit_test.hpp>

#include <core/radio_s.hpp>
#include <cfloat>


class DoubleEveryOther : public FilterChainElement
{
public:
    virtual ~DoubleEveryOther() {}
    DoubleEveryOther() :
        m_output(),
        m_input(),
        m_counter(0)
    {}
    bool input(const filter_io_t &data) override
    {
        m_input = data;
        m_counter++;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        if (m_counter == 2) {
            data = m_output;
            m_counter = 0;
            return true;
        }
        return false;
    }

    void tick(void) override
    {
        if (m_counter == 2) {
            m_output.type = IO_TYPE_COMPLEX_DOUBLE;
            m_output.rf = 2.0 * m_input.rf;
        }
    }
private:
    filter_io_t m_output;
    filter_io_t m_input;
    int         m_counter;
};

class RFNormToInteger : public FilterChainElement
{
public:
    virtual ~RFNormToInteger() {}
    RFNormToInteger() :
        m_input(),
        m_byte(0),
        m_didReceiveInput(false)
    {}
    bool input(const filter_io_t &data) override
    {
        m_input = data;
        m_didReceiveInput = true;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        if (m_didReceiveInput) {
            m_didReceiveInput = false;
            data.type = IO_TYPE_BYTE;
            data.byte = m_byte;
            return true;
        }
        return false;
    }

    void tick(void) override
    {
        if (m_didReceiveInput) {
            m_byte = static_cast<uint8_t>(sqrt(m_input.rf.real()*m_input.rf.real() + m_input.rf.imag()*m_input.rf.imag()));
        }
    }
private:
    filter_io_t m_input;
    uint8_t     m_byte;
    bool        m_didReceiveInput;
};

class IntegerToRF : public FilterChainElement
{
public:
    virtual ~IntegerToRF() {}
    IntegerToRF() :
        m_output(),
        m_byte(0),
        m_didReceiveInput(false)
    {}
    bool input(const filter_io_t &data) override
    {
        m_byte = data.byte;
        m_didReceiveInput = true;
        return true;
    }

    bool output(filter_io_t &data) override
    {
        if (m_didReceiveInput) {
            m_didReceiveInput = false;
            data = m_output;
            return true;
        }
        return false;
    }

    void tick(void) override
    {
        if (m_didReceiveInput) {
            m_output.type = IO_TYPE_COMPLEX_DOUBLE;
            m_output.rf = std::complex<double>(static_cast<double>(m_byte / 2), static_cast<double>(m_byte / 2));
        }
    }
private:
    filter_io_t m_output;
    uint8_t     m_byte;
    bool        m_didReceiveInput;
};


CSIM_TEST_SUITE_BEGIN(RadioTests)

CSIM_TEST_CASE(FILTER_CHAIN_IS_PROPERLY_CHAINED)
{

    radio_config_t config;
    config.position = Vector2d(0,0);
    config.id = 100;

    RFNormToInteger *rfnti = new RFNormToInteger();
    DoubleEveryOther *deo1 = new DoubleEveryOther();
    DoubleEveryOther *deo2 = new DoubleEveryOther();

    FilterChain demodulation_chain;
    demodulation_chain = *rfnti + *deo2 + *deo1;

    IntegerToRF *itrf = new IntegerToRF();
    deo1 = new DoubleEveryOther();
    deo2 = new DoubleEveryOther();

    FilterChain modulation_chain;
    modulation_chain = *deo2 + *deo1 + *itrf;

    RadioS *radio = new RadioS(config, modulation_chain, demodulation_chain);
    uint8_t byte = 0;
    std::complex<double> sample(0.5, 0.5);
    bool didOutput = true;

    /* Demodulation */
    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxByte(byte);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxByte(byte);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxByte(byte);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxByte(byte);
    BOOST_CHECK_EQUAL(didOutput, true);
    BOOST_CHECK_EQUAL(byte, 2);

    /* Modulation */
    radio->txByte(10);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->txByte(10);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->txByte(10);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->txByte(10);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, true);
    BOOST_CHECK_CLOSE(sample.real(), 20.0, DBL_EPSILON);
    BOOST_CHECK_CLOSE(sample.imag(), 20.0, DBL_EPSILON);

}

CSIM_TEST_SUITE_END()
