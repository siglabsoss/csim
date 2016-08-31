#include <test/unit_test.hpp>

#define private public
#include <filters/biquad.hpp>

CSIM_TEST_SUITE_BEGIN(BiquadVerification)

CSIM_TEST_CASE(BASIC_OPERATION)
{
    Biquad bi;
    //bi.init(0.00482390, 0.003322259, 3.9999, 4.0, 49.2465);
    bi.init(2.00482390, 2.003322259, 3.9999, 4.0, 49.2465);
    FixedComplexNorm16 input(0.9999, 0.9999);
    filter_io_t data;
    data = input;
    for (size_t i = 0; i < 3; i++) {
        bi.input(data);
        bi.tick();
        filter_io_t output;
        BOOST_CHECK(bi.output(output) == true);
        std::cout << output << std::endl;
    }
}

CSIM_TEST_SUITE_END()
