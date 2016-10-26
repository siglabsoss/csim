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

class RFNormToBit : public FilterChainElement
{
public:
    virtual ~RFNormToBit() {}
    RFNormToBit() :
        m_input(),
        m_bit(0),
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
            data.type = IO_TYPE_BIT;
            data.byte = m_bit;
            return true;
        }
        return false;
    }

    void tick(void) override
    {
        if (m_didReceiveInput) {
            m_bit = static_cast<bool>(sqrt(m_input.rf.real()*m_input.rf.real() + m_input.rf.imag()*m_input.rf.imag()) > 0.5);
        }
    }
private:
    filter_io_t m_input;
    bool        m_bit;
    bool        m_didReceiveInput;
};

class IntegerToRF : public FilterChainElement
{
public:
    virtual ~IntegerToRF() {}
    IntegerToRF() :
        m_output(),
        m_bit(0),
        m_didReceiveInput(false)
    {}
    bool input(const filter_io_t &data) override
    {
        assert(data.type == IO_TYPE_BIT);
        m_bit = data.byte;
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
            m_output.rf = ComplexDouble(static_cast<double>(m_bit), static_cast<double>(m_bit));
        }
    }
private:
    filter_io_t m_output;
    bool        m_bit;
    bool        m_didReceiveInput;
};


CSIM_TEST_SUITE_BEGIN(RadioTests)

CSIM_TEST_CASE(FILTER_CHAIN_IS_PROPERLY_CHAINED)
{

    radio_config_t config;
    config.position = Vector2d(0,0);
    config.id = 100;

    RFNormToBit *rfnti = new RFNormToBit();
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
    bool bit = 0;
    ComplexDouble sample(0.5, 0.5);
    bool didOutput = true;

    /* Demodulation */
    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxBit(bit);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxBit(bit);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxBit(bit);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->rxWave(sample);
    radio->tick();
    didOutput = radio->rxBit(bit);
    BOOST_CHECK_EQUAL(didOutput, true);
    BOOST_CHECK_EQUAL(bit, 1);

    /* Modulation */
    radio->txBit(1);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->txBit(1);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->txBit(1);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, false);

    radio->txBit(1);
    radio->tick();
    didOutput = radio->txWave(sample);
    BOOST_CHECK_EQUAL(didOutput, true);
    BOOST_CHECK_CLOSE(sample.real(), 4.0, DBL_EPSILON);
    BOOST_CHECK_CLOSE(sample.imag(), 4.0, DBL_EPSILON);

}

CSIM_TEST_SUITE_END()
