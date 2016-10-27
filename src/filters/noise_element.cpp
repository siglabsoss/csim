
#include <filters/noise_element.hpp>
#include <utils/plotter.hpp>

NoiseElement::~NoiseElement()
{
}
//we want a gaussian distribution with mean = 0 and stddev = 10^(-ebn0.20)
NoiseElement::NoiseElement(double ebn0) :
    FilterChainElement(std::string("NoiseElement")),
    m_noiseGenerator(pow(pow(10, -ebn0/20.0), 2)), //more info here: http://read.pudn.com/downloads152/doc/comm/664022/ber.pdf
    m_inputValid(false)
{
}

bool NoiseElement::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT || data.type == IO_TYPE_COMPLEX_DOUBLE);

    m_input = data.toComplexDouble();
    if (std::abs(m_input) > 0.01) {
        m_inputValid = true;
    }
    return true;
}

bool NoiseElement::output(filter_io_t &data)
{
    if (m_inputValid) {
        m_inputValid = false;
        data = m_input + m_noiseGenerator.getNext();
        return true;
    }
    return false;
}

void NoiseElement::tick(void)
{

}
