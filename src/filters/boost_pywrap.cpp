#include "boost_pywrap.hpp"


using namespace std;

#define PYWRAP_FILTER_DEFINITIONS(cls) \
void cls::tick() \
{ \
	m_wrap->tick(); \
} \
bool cls::input(const filter_io_t &data) \
{ \
	return m_wrap->input(data); \
} \
bool cls::output(filter_io_t &data) \
{ \
	return m_wrap->output(data); \
} \





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
{
	const double MIXER_FREQ = 0.16;
	// These are the coefficients designed by daryl@siglabs.com for the up converter's band pass filter
	vector<double> up2Coeffs = {-2, 0, 7, -0, -19, 0, 46, -0, -98, 0, 188, -1, -337, 1, 572, -2, -924, 2, 1436, -3, -2163, 4, 3182, -5, -4611, 6, 6660, -7, -9770, 8, 15085, -9, -26825, 9, 83113, 131070, 83113, 9, -26825, -9, 15085, 8, -9770, -7, 6660, 6, -4611, -5, 3182, 4, -2163, -3, 1436, 2, -924, -2, 572, 1, -337, -1, 188, 0, -98, -0, 46, 0, -19, -0, 7, 0, -2};
	vector<double> up5Coeffs = {-1, -1, -2, -1, 0, 3, 7, 8, 6, -2, -14, -27, -33, -24, 5, 48, 90, 107, 76, -12, -139, -257, -300, -211, 26, 354, 647, 743, 516, -50, -807, -1459, -1654, -1139, 85, 1678, 3012, 3383, 2318, -132, -3259, -5834, -6526, -4475, 184, 6106, 11003, 12392, 8612, -233, -11830, -21980, -25707, -18865, 267, 29986, 65194, 98544, 122410, 131070, 122410, 98544, 65194, 29986, 267, -18865, -25707, -21980, -11830, -233, 8612, 12392, 11003, 6106, 184, -4475, -6526, -5834, -3259, -132, 2318, 3383, 3012, 1678, 85, -1139, -1654, -1459, -807, -50, 516, 743, 647, 354, 26, -211, -300, -257, -139, -12, 76, 107, 90, 48, 5, -24, -33, -27, -14, -2, 6, 8, 7, 3, 0, -1, -2, -1, -1};

	m_wrap = new DigitalUpConverter(-MIXER_FREQ, up2Coeffs, up5Coeffs);

//	for (std::vector<double>::const_iterator i = up2Coeffs.begin(); i != up2Coeffs.end(); ++i)
//	    std::cout << *i << ' ';
}

PYWRAP_FILTER_DEFINITIONS(WrapDigitalUpConverter);


WrapDigitalDownConverter::WrapDigitalDownConverter()
{
	const double MIXER_FREQ = 0.16;

	// These are the coefficients designed by daryl@siglabs.com for the down converter's band pass filter
	vector<double> down2Coeffs = {-7, 0, 18, 0, -40, 0, 81, 0, -151, 0, 266, 0, -443, 0, 708, 0, -1091, 0, 1631, 0, -2381, 0, 3412, 0, -4841, 0, 6876, 0, -9957, 0, 15230, 0, -26917, 0, 83145, 131071, 83145, 0, -26917, 0, 15230, 0, -9957, 0, 6876, 0, -4841, 0, 3412, 0, -2381, 0, 1631, 0, -1091, 0, 708, 0, -443, 0, 266, 0, -151, 0, 81, 0, -40, 0, 18, 0, -7};
	vector<double> down5Coeffs = {0, -5, -6, -4, 0, 5, 8, 9, 6, 0, -6, -11, -12, -8, 0, 9, 16, 17, 11, 0, -13, -22, -23, -15, 0, 17, 29, 31, 20, 0, -23, -39, -41, -27, 0, 30, 52, 55, 36, 0, -39, -67, -71, -46, 0, 51, 87, 91, 59, 0, -65, -111, -117, -76, 0, 83, 141, 147, 95, 0, -104, -177, -185, -119, 0, 130, 220, 230, 148, 0, -161, -271, -283, -182, 0, 197, 333, 347, 223, 0, -241, -405, -421, -271, 0, 292, 491, 509, 327, 0, -351, -590, -612, -393, 0, 421, 706, 732, 469, 0, -502, -841, -871, -558, 0, 596, 998, 1032, 660, 0, -705, -1179, -1219, -779, 0, 831, 1389, 1435, 917, 0, -977, -1633, -1686, -1077, 0, 1146, 1916, 1978, 1263, 0, -1345, -2247, -2321, -1481, 0, 1578, 2638, 2725, 1740, 0, -1856, -3104, -3208, -2050, 0, 2191, 3668, 3796, 2429, 0, -2603, -4365, -4526, -2902, 0, 3125, 5254, 5463, 3513, 0, -3810, -6431, -6716, -4339, 0, 4757, 8080, 8495, 5530, 0, -6171, -10590, -11262, -7426, 0, 8548, 14946, 16242, 10983, 0, -13488, -24604, -28172, -20346, 0, 30592, 66058, 99149, 122600, 131071, 122600, 99149, 66058, 30592, 0, -20346, -28172, -24604, -13488, 0, 10983, 16242, 14946, 8548, 0, -7426, -11262, -10590, -6171, 0, 5530, 8495, 8080, 4757, 0, -4339, -6716, -6431, -3810, 0, 3513, 5463, 5254, 3125, 0, -2902, -4526, -4365, -2603, 0, 2429, 3796, 3668, 2191, 0, -2050, -3208, -3104, -1856, 0, 1740, 2725, 2638, 1578, 0, -1481, -2321, -2247, -1345, 0, 1263, 1978, 1916, 1146, 0, -1077, -1686, -1633, -977, 0, 917, 1435, 1389, 831, 0, -779, -1219, -1179, -705, 0, 660, 1032, 998, 596, 0, -558, -871, -841, -502, 0, 469, 732, 706, 421, 0, -393, -612, -590, -351, 0, 327, 509, 491, 292, 0, -271, -421, -405, -241, 0, 223, 347, 333, 197, 0, -182, -283, -271, -161, 0, 148, 230, 220, 130, 0, -119, -185, -177, -104, 0, 95, 147, 141, 83, 0, -76, -117, -111, -65, 0, 59, 91, 87, 51, 0, -46, -71, -67, -39, 0, 36, 55, 52, 30, 0, -27, -41, -39, -23, 0, 20, 31, 29, 17, 0, -15, -23, -22, -13, 0, 11, 17, 16, 9, 0, -8, -12, -11, -6, 0, 6, 9, 8, 5, 0, -4, -6, -5};

	m_wrap = new DigitalDownConverter(MIXER_FREQ, down2Coeffs, down5Coeffs);
}

PYWRAP_FILTER_DEFINITIONS(WrapDigitalDownConverter);






WrapNoiseElement::WrapNoiseElement(double variance)
{
	m_wrap = new NoiseElement(variance);
}

PYWRAP_FILTER_DEFINITIONS(WrapNoiseElement);







WrapFilterChain::WrapFilterChain()
{
	m_wrap = new FilterChain();

	m_links[0] = new NoiseElement(0.000);
	m_links[1] = new NoiseElement(0.000);

//	m_links[0] = m_filters[0];
//	m_links[1] = m_filters[1];


	*m_wrap = *m_links[0] + *m_links[1];

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






