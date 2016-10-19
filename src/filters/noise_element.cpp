
#include <filters/noise_element.hpp>
#include <utils/plotter.hpp>

NoiseElement::~NoiseElement(){
//    const plotter &plot = plotter::get();
//    vector<double> vec;
//
//    for(unsigned i = 0; i < buf.size(); i++)
//    {
//        vec.push_back(std::real(buf[i]).to_double());
//    }


//    plot.nplot(buf2, "NoiseElement");
}

NoiseElement::NoiseElement(double ebn0) :
    FilterChainElement(std::string("NoiseElement")),
    m_inputValid(false),
    m_ebn0(ebn0),
    m_scalar(pow(10, -ebn0/20.0))   // 10^(-Eb_N0_dB/20)
{
}

bool NoiseElement::input(const filter_io_t &data)
{
//    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW || data.type == IO_TYPE_COMPLEX_DOUBLE);
    m_inputValid = true;
    m_input = data;
//    buf.push_back(data.fcn32);
    buf2.push_back(std::real(data.rf));
//    cout << data.type << end;
    return true;
}

bool NoiseElement::output(filter_io_t &data)
{
    m_inputValid = false;

    ComplexDouble noise = m_noise.getNext() * 0.707106781186547;  // scaled by 1/sqrt(2) to get to 0DB noise
    noise *= m_scalar;  // scaled by ebn0 value

//    m_input.fcn32 += FixedComplex32(1,0);
    data = m_input.toComplexDouble() + noise;
//    data = m_carrier * m_input;
    return true;
}

void NoiseElement::tick(void)
{

}
