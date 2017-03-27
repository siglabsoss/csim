#include <test/unit_test.hpp>
#include <types/fixedcomplex.hpp>

CSIM_TEST_SUITE_BEGIN(SLFixedComplexTests)

CSIM_TEST_CASE(COMPLEX_CONJUGATE)
{
    SLFixComplex fc(8,
                    2,
                    SLFixPoint::QUANT_RND_HALF_UP,
                    SLFixPoint::OVERFLOW_SATURATE);

    fc.set(1.5223532, 0.5764565);

    BOOST_CHECK_EQUAL(**fc,                    fc);
    BOOST_CHECK_EQUAL(**fc,                    fc);

    BOOST_CHECK_EQUAL((*fc).real(),            fc.real());
    BOOST_CHECK_EQUAL((*fc).imag().to_int64(), -fc.imag().to_int64());
}

CSIM_TEST_SUITE_END()
