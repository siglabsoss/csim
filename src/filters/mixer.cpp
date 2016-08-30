
#include <filters/mixer.hpp>

Mixer::~Mixer() {}

Mixer::Mixer(uint32_t samplesPerPeriod) :
    FilterChainElement(std::string("Mixer")),
    m_count(0),
    m_samplesPerPeriod(samplesPerPeriod),
    m_inputValid(false)
{
}

bool Mixer::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXED_COMPLEX);
    m_inputValid = true;
    m_input = data.toComplexDouble();
    return true;
}

bool Mixer::output(filter_io_t &data)
{
    if (m_inputValid) {
        m_inputValid = false;
        data = m_carrier * m_input;
        return true;
    }
    return false;
}

void Mixer::tick(void)
{
    if (m_inputValid) {
        double theta = (((2 * M_PI) / m_samplesPerPeriod) * m_count);
        m_carrier = ComplexDouble(cos(theta), sin(theta));
        m_count++;
        if (m_count >= m_samplesPerPeriod) {
            m_count -= m_samplesPerPeriod;
        }
    }
}

