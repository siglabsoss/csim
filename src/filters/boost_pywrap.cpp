#include "boost_pywrap.hpp"


using namespace std;


PyWrap::PyWrap(int a)
{
	this->m_val = a;
}

void PyWrap::set(int a)
{
	this->m_val = a;
}
int PyWrap::get(void)
{
	return this->m_val;
}

WrapDigitalUpConverter::WrapDigitalUpConverter()
{	const double MIXER_FREQ = 0.16;

	vector<double> up2Coeffs = {-2, 0, 7, -0, -19, 0, 46, -0, -98, 0, 188, -1, -337, 1, 572, -2, -924, 2, 1436, -3, -2163, 4, 3182, -5, -4611, 6, 6660, -7, -9770, 8, 15085, -9, -26825, 9, 83113, 131070, 83113, 9, -26825, -9, 15085, 8, -9770, -7, 6660, 6, -4611, -5, 3182, 4, -2163, -3, 1436, 2, -924, -2, 572, 1, -337, -1, 188, 0, -98, -0, 46, 0, -19, -0, 7, 0, -2};
	vector<double> up5Coeffs = {-1, -1, -2, -1, 0, 3, 7, 8, 6, -2, -14, -27, -33, -24, 5, 48, 90, 107, 76, -12, -139, -257, -300, -211, 26, 354, 647, 743, 516, -50, -807, -1459, -1654, -1139, 85, 1678, 3012, 3383, 2318, -132, -3259, -5834, -6526, -4475, 184, 6106, 11003, 12392, 8612, -233, -11830, -21980, -25707, -18865, 267, 29986, 65194, 98544, 122410, 131070, 122410, 98544, 65194, 29986, 267, -18865, -25707, -21980, -11830, -233, 8612, 12392, 11003, 6106, 184, -4475, -6526, -5834, -3259, -132, 2318, 3383, 3012, 1678, 85, -1139, -1654, -1459, -807, -50, 516, 743, 647, 354, 26, -211, -300, -257, -139, -12, 76, 107, 90, 48, 5, -24, -33, -27, -14, -2, 6, 8, 7, 3, 0, -1, -2, -1, -1};

	m_duc = new DigitalUpConverter(-MIXER_FREQ, up2Coeffs, up5Coeffs);

//	for (std::vector<double>::const_iterator i = up2Coeffs.begin(); i != up2Coeffs.end(); ++i)
//	    std::cout << *i << ' ';
}

void WrapDigitalUpConverter::tick()
{
	m_duc->tick();
}

bool WrapDigitalUpConverter::input(const filter_io_t &data)
{
	return m_duc->input(data);
}

bool WrapDigitalUpConverter::output(filter_io_t &data)
{
	return m_duc->output(data);
}










WrapNoiseElement::WrapNoiseElement(double variance)
{
	m_wrap = new NoiseElement(variance);
}

void WrapNoiseElement::tick()
{
	m_wrap->tick();
}

bool WrapNoiseElement::input(const filter_io_t &data)
{
	return m_wrap->input(data);
}

bool WrapNoiseElement::output(filter_io_t &data)
{
	return m_wrap->output(data);
}






WrapFilterChain::WrapFilterChain()
{
	m_wrap = new FilterChain();

	m_wrap1 = new NoiseElement(0.000);
	m_wrap2 = new NoiseElement(0.000);


	*m_wrap = *m_wrap1+*m_wrap2;
}


void WrapFilterChain::tick()
{
	m_wrap->tick();
}

bool WrapFilterChain::input(const filter_io_t &data)
{
	return m_wrap->input(data);
}

bool WrapFilterChain::output(filter_io_t &data)
{
	return m_wrap->output(data);
}






