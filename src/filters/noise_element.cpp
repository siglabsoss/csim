
#include <filters/noise_element.hpp>
#include <utils/plotter.hpp>

NoiseElement::~NoiseElement()
{
}

NoiseElement::NoiseElement(double ebn0) :
    FilterChainElement(std::string("NoiseElement")),
    m_inputValid(false),
    m_scalar(pow(10, -ebn0/20.0))   // 10^(-Eb_N0_dB/20)
{
}

bool NoiseElement::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT || data.type == IO_TYPE_COMPLEX_DOUBLE);
    m_inputValid = true;
    m_input = data;
    return true;
}

bool NoiseElement::output(filter_io_t &data)
{
    if (m_inputValid) {
        m_inputValid = false;
        ComplexDouble noise = m_noiseGenerator.getNext() * (m_scalar * M_SQRT1_2);  // scaled by 1/sqrt(2) to get to 0DB noise and then by enb0 term
        data = m_input.toComplexDouble() + noise;
        return true;
    }
    return false;
}

void NoiseElement::tick(void)
{

}
