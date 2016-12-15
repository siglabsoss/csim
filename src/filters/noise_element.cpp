
#include <filters/noise_element.hpp>
#include <filters/fft.hpp>
#include <utils/plotter.hpp>

NoiseElement::~NoiseElement()
{
}
//we want a gaussian distribution with mean = 0 and stddev = 10^(-ebn0/20)
NoiseElement::NoiseElement(double variance) :
    FilterChainElement(std::string("NoiseElement")),
    m_noiseGenerator(variance),
    m_inputValid(false)
{
}

bool NoiseElement::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_COMPLEX_FIXPOINT || data.type == IO_TYPE_COMPLEX_DOUBLE);

    m_input = data.toComplexDouble();
    m_inputValid = true;
    return true;
}

bool NoiseElement::output(filter_io_t &data)
{
    if (m_inputValid) {
        m_inputValid = false;
        ComplexDouble output = m_input + m_noiseGenerator.getNext();
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(FFT_INPUT_FORMAT);
        data.fc.real(output.real());
        data.fc.imag(output.imag());
        return true;
    }
    return false;
}

void NoiseElement::tick(void)
{

}
