#include <test/unit_test.hpp>
#include <core/filter_probe.hpp>

#include <cfloat>

class TestProbe : public FilterProbe
{
public:
    TestProbe() :
        FilterProbe("DummyProbe", 100)
    {}
    void tick() override
    {

    }
};


CSIM_TEST_SUITE_BEGIN(FilterProbeTests)

CSIM_TEST_CASE(PROPER_PASSTHROUGH)
{
    TestProbe probe;
    filter_io_t data1, data2, output;
    data1.type = IO_TYPE_FIXED_COMPLEX;
    data1.fc = FixedComplex32(0.75, 0.1);

    data2.type = IO_TYPE_FIXED_COMPLEX;
    data2.fc = FixedComplex32(0.45, 0.12);

    //outputs false as long as nothing was input
    BOOST_CHECK(probe.output(output) == false);
    BOOST_CHECK(probe.output(output) == false);
    BOOST_CHECK(probe.output(output) == false);

    //after input, output returns true once and only once
    BOOST_CHECK(probe.input(data1) == true);
    BOOST_CHECK(probe.output(output) == true);

    BOOST_CHECK_CLOSE(output.toComplexDouble().real(), 0.75, 0.1);
    BOOST_CHECK_CLOSE(output.toComplexDouble().imag(), 0.1, 0.1);

    BOOST_CHECK(probe.output(output) == false);
    BOOST_CHECK(probe.output(output) == false);

    //back to back inputs still yields only one output
    BOOST_CHECK(probe.input(data1) == true);
    BOOST_CHECK(probe.input(data2) == true);
    BOOST_CHECK(probe.output(output) == true);

    BOOST_CHECK_CLOSE(output.toComplexDouble().real(), 0.45, 0.1);
    BOOST_CHECK_CLOSE(output.toComplexDouble().imag(), 0.12, 0.1);

    BOOST_CHECK(probe.output(output) == false);
}

CSIM_TEST_SUITE_END()
