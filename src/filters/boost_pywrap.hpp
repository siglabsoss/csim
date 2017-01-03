#pragma once
#include <core/filter_chain.hpp>
#include <core/logger.hpp>

#include <types/circularbuffer.hpp>

#include <filters/fft.hpp>
#include <filters/demapper.hpp>
#include <filters/mapper.hpp>
#include <filters/noise_element.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <filters/cyclic_prefix.hpp>
#include <filters/frame_sync.hpp>
#include <filters/channel_equalizer.hpp>
#include <filters/ldpc_encode.hpp>
#include <filters/ldpc_decoder.hpp>
#include <filters/puncture.hpp>
#include <filters/depuncture.hpp>
#include <filters/scrambler_block.hpp>

#include <utils/utils.hpp>
#include <utils/ldpc_utils.hpp>

#include <probes/sample_count_trigger.hpp>
#include <filters/ddc_duc/duc.hpp>

#include <3rd/json/json.h>

#include <mathlib/complex_gaussian_noise.hpp>

#include <cstdint>
#include <types/fixedpoint.hpp>

class PyWrap
{

public:
	void set(int);
	int get(void);
	PyWrap(int);
private:
	int m_val;
};


class WrapDigitalUpConverter
{
public:
	WrapDigitalUpConverter();
	bool input(const filter_io_t &data);
	bool output(filter_io_t &data);
	void tick(void);
private:
	DigitalUpConverter *m_duc;
};

class WrapNoiseElement
{
public:
	WrapNoiseElement(double variance);
	bool input(const filter_io_t &data);
	bool output(filter_io_t &data);
	void tick(void);
private:
	NoiseElement *m_wrap;
};

class WrapFilterChain
{
public:
	WrapFilterChain();
	bool input(const filter_io_t &data);
	bool output(filter_io_t &data);
	void tick(void);
private:
	static constexpr size_t max_chain = 10;
	FilterChain *m_wrap;
	FilterChainElement *m_links[max_chain];
	void add(FilterChainElement &rhs);
//	std::vector<FilterChainElement*> m_links;
//	FilterChainElement *m_wrap1;
//	FilterChainElement *m_wrap2;
};

void unboundd(void)
{
	std::cout << "from inside" << std::endl;
}


#ifdef USE_PYTHON_BINDINGS
#include <boost/python.hpp>
using namespace boost::python;







#define PP_XSTR(s) PP_STR(s)
#define PP_STR(s) #s
#define PP_STRIP(...) __VA_ARGS__


#define PYWRAP_FILTER_HEADER(cls, signature)                                    \
	class_<cls>(PP_STR(cls), init<signature>())   \
	.def("tick", &cls::tick)   \
	.def("input", &cls::input)   \
	.def("output", &cls::output)





BOOST_PYTHON_MODULE(libboost_pywrap)
{
    def("unboundd", unboundd);

    class_<PyWrap>("PyWrap", init<int>())
            .def("get", &PyWrap::get)
            .def("set", &PyWrap::set)
        ;

    double (ComplexDouble::*real_getter)() const = &ComplexDouble::real;
    void (ComplexDouble::*real_setter)(double) = &ComplexDouble::real;

    double (ComplexDouble::*imag_getter)() const = &ComplexDouble::imag;
    void (ComplexDouble::*imag_setter)(double) = &ComplexDouble::imag;

    class_<ComplexDouble>("ComplexDouble", init<double,double>())
    		.def("real", real_getter)
    		.def("real", real_setter)
    		.def("imag", imag_getter)
    		.def("imag", imag_setter)
    		;

    class_<ComplexGaussianNoise>("ComplexGaussianNoise", init<>())
    		.def(init<double>())
    		.def("getNext", &ComplexGaussianNoise::getNext)
    		;

    // overrides require making a function pointer like this
    void (SLFixPoint::*slfp_set_format_full)(size_t, ssize_t, SLFixPoint::quant_mode_t, SLFixPoint::overflow_mode_t) = &SLFixPoint::setFormat;

    double (SLFixPoint::*slfp_to_double)(void) const = &SLFixPoint::to_double;

    void (SLFixPoint::*slfp_set)(double) = &SLFixPoint::set;

    class_<SLFixPoint>("SLFixPoint", init<>())
    		.def(init<size_t, ssize_t>())
    		.def(init<size_t, ssize_t, SLFixPoint::quant_mode_t, SLFixPoint::overflow_mode_t>())
    		.def("setFormat", slfp_set_format_full)
    		.def("to_double", slfp_to_double)
    		.def("set", slfp_set)
    		;

    // getters
    SLFixPoint (SLFixComplex::*slfc_real_getter)() const = &SLFixComplex::real;
    void (SLFixComplex::*slfc_real_slfp_setter)(const SLFixPoint&) = &SLFixComplex::real;
    void (SLFixComplex::*slfc_real_double_setter)(double) = &SLFixComplex::real;

    // setters
    SLFixPoint (SLFixComplex::*slfc_imag_getter)() const = &SLFixComplex::imag;
    void (SLFixComplex::*slfc_imag_slfp_setter)(const SLFixPoint&) = &SLFixComplex::imag;
    void (SLFixComplex::*slfc_imag_double_setter)(double) = &SLFixComplex::imag;

    // format
    void (SLFixComplex::*slfc_set_format_full)(size_t, ssize_t, SLFixPoint::quant_mode_t, SLFixPoint::overflow_mode_t) = &SLFixComplex::setFormat;

    // to complex double
    ComplexDouble (SLFixComplex::*slfc_to_complex_double)(void) const = &SLFixComplex::toComplexDouble;

    class_<SLFixComplex>("SLFixComplex", init<>())
			.def("real", slfc_real_getter)
			.def("real", slfc_real_slfp_setter)
			.def("real", slfc_real_double_setter)
			.def("imag", slfc_imag_getter)
			.def("imag", slfc_imag_slfp_setter)
			.def("imag", slfc_imag_double_setter)
			.def("setFormat", slfc_set_format_full)
			.def("toComplexDouble", slfc_to_complex_double)
    		;

    class_<filter_io_t>("filter_io_t", init<>())
			.def(init<uint8_t>())
			.def(init<ComplexDouble>())
    		.def("toComplexDouble", &filter_io_t::toComplexDouble)
    		;

    enum_<SLFixPoint::quant_mode_t>("SLFixPoint_quant_mode_t")
            .value("QUANT_TRUNCATE", SLFixPoint::quant_mode_t::QUANT_TRUNCATE)
            .value("QUANT_RND_HALF_UP", SLFixPoint::quant_mode_t::QUANT_RND_HALF_UP)
            ;

    enum_<SLFixPoint::overflow_mode_t>("SLFixPoint_overflow_mode_t")
                .value("OVERFLOW_WRAP_AROUND", SLFixPoint::overflow_mode_t::OVERFLOW_WRAP_AROUND)
                .value("OVERFLOW_SATURATE", SLFixPoint::overflow_mode_t::OVERFLOW_SATURATE)
			;

    class_<WrapFilterChain>("WrapFilterChain")
			.def("tick", &WrapFilterChain::tick)
			.def("input", &WrapFilterChain::input)
			.def("output", &WrapFilterChain::output)
		;


    class_<WrapDigitalUpConverter>("WrapDigitalUpConverter", init<>())
    		.def("tick", &WrapDigitalUpConverter::tick)
    		.def("input", &WrapDigitalUpConverter::input)
		;





    PYWRAP_FILTER_HEADER(WrapNoiseElement,double);


}

#endif
