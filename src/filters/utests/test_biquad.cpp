#include <test/unit_test.hpp>

#define private public
#include <filters/biquad.hpp>

CSIM_TEST_SUITE_BEGIN(BiquadVerification)

CSIM_TEST_CASE(BASIC_OPERATION)
{
    Biquad bi;
    bi.init(1.0, 1.54712277573, 1.0, -0.11670051447, 0.20472156970);
    FixedComplexNorm16 input(0.3, 0.3);
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
