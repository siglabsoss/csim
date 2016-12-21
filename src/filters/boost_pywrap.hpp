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
//	bool input(const filter_io_t &data);
//	bool output(filter_io_t &data);
	void tick(void);
private:
	DigitalUpConverter *m_duc;
	SLFixPoint junk; // we need to use these types to make sure they are exported to python
	filter_io_t junk1;
	//FilterChain *m_chain;
};

void unboundd(void)
{
	std::cout << "from inside" << std::endl;
}


#ifdef USE_PYTHON_BINDINGS
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(libboost_pywrap)
{
    def("unboundd", unboundd);

    class_<PyWrap>("PyWrap", init<int>())
            .def("get", &PyWrap::get)
            .def("set", &PyWrap::set)
        ;

    class_<WrapDigitalUpConverter>("WrapDigitalUpConverter", init<>())
    		.def("tick", &WrapDigitalUpConverter::tick)
		;
}

#endif
