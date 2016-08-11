#pragma once

class LinearGainAmplifier : public FilterChainElement
{
public:
    virtual ~LinearGainAmplifier() {}
    LinearGainAmplifier(double gain) :
        FilterChainElement("LGA"),
        m_sample(),
        m_gain(gain),
        m_validInput(false)
    {}

    bool input(const filter_io_t &data) override
    {
        m_validInput = true;
        m_sample = data;
        return true;
    }
    bool output(filter_io_t &data) override
    {
        if (!m_validInput) {
            return false;
        }
        m_validInput = false;
        switch(m_sample.type) {
            case IO_TYPE_NULL:
                break;
            case IO_TYPE_COMPLEX_DOUBLE:
                data.rf.real(m_sample.rf.real() * m_gain);
                data.rf.imag(m_sample.rf.imag() * m_gain);
                break;
            case IO_TYPE_FIXED_COMPLEX_16:
                data.fc.real = (m_sample.fc.real * m_gain);
                data.fc.imag = (m_sample.fc.imag * m_gain);
                break;
            case IO_TYPE_FIXED_COMPLEX_32:
                data.fc32.real = (m_sample.fc32.real * m_gain);
                data.fc32.imag = (m_sample.fc32.imag * m_gain);
                break;
            case IO_TYPE_FIXED_COMPLEX_16_NEW:
                data.fcn.real(m_sample.fcn.real() * m_gain);
                data.fcn.imag(m_sample.fcn.imag() * m_gain);
                break;
            case IO_TYPE_FIXED_COMPLEX_32_NEW:
                data.fcn32.real(m_sample.fcn32.real() * m_gain);
                data.fcn32.imag(m_sample.fcn32.imag() * m_gain);
                break;
            case IO_TYPE_BYTE:
                break;
        }
        data.type = m_sample.type;
        return true;
    }

    void tick(void) override
    {
    }

private:
    filter_io_t m_sample;
    double m_gain;
    bool m_validInput;
};
