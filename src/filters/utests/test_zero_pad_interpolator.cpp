#include <test/unit_test.hpp>
#include <filters/zero_pad_interpolator.hpp>
#include <cfloat>

CSIM_TEST_SUITE_BEGIN(InterpolationFunctionality)

CSIM_TEST_CASE(PROPERLY_ZERO_PADDED)
{
    size_t chunkSize = 1024;
    ZeroPadInterpolator interp(chunkSize);
    filter_io_t data1, data2, output;
    data1 = FixedComplex32(1.0, 1.0);
    data2 = FixedComplex32(-1.0,-1.0);

    //Load up the interpolator with one less than chunk size elements and expect no output
    for (size_t i = 0; i < chunkSize - 1; i++) {
        if (i < chunkSize / 2) {
            BOOST_CHECK(interp.input(data1) == true);
        }else {
            BOOST_CHECK(interp.input(data2) == true);
        }
        interp.tick();
        BOOST_CHECK(interp.output(output) == false);
    }

    interp.input(data2);
    interp.tick();

    size_t leftSectionEnd = chunkSize / 2;
    size_t middleSectionBegin = chunkSize / 2;
    size_t middleSectionEnd = middleSectionBegin + chunkSize;
    size_t rightSectionBegin = middleSectionEnd;
    size_t rightSectionEnd = chunkSize * 2;

    for (size_t i = 0; i < leftSectionEnd; i++) {
        BOOST_CHECK(interp.output(output) == true);
        BOOST_REQUIRE_CLOSE(output.fcn32.real().to_double(), 1.0, DBL_EPSILON);
        BOOST_REQUIRE_CLOSE(output.fcn32.imag().to_double(), 1.0, DBL_EPSILON);
        interp.tick();
    }

    for (size_t i = middleSectionBegin; i < middleSectionEnd; i++) {
        BOOST_CHECK(interp.output(output) == true);
        BOOST_REQUIRE_CLOSE(output.fcn32.real().to_double(), 0.0, DBL_EPSILON);
        BOOST_REQUIRE_CLOSE(output.fcn32.imag().to_double(), 0.0, DBL_EPSILON);
        interp.tick();
    }

    for (size_t i = rightSectionBegin; i < rightSectionEnd; i++) {
        BOOST_CHECK(interp.output(output) == true);
        BOOST_REQUIRE_CLOSE(output.fcn32.real().to_double(), -1.0, DBL_EPSILON);
        BOOST_REQUIRE_CLOSE(output.fcn32.imag().to_double(), -1.0, DBL_EPSILON);
        interp.tick();
    }

    //We should have drained the output by now
    BOOST_CHECK(interp.output(output) == false);


    //Load up the interpolator with one less than chunk size elements and expect no output
    for (size_t i = 0; i < chunkSize - 1; i++) {
        if (i < chunkSize / 2) {
            BOOST_CHECK(interp.input(data1) == true);
        }else {
            BOOST_CHECK(interp.input(data2) == true);
        }
        interp.tick();
        BOOST_CHECK(interp.output(output) == false);
    }

    interp.input(data2);
    interp.tick();


    for (size_t i = 0; i < leftSectionEnd; i++) {
        BOOST_CHECK(interp.output(output) == true);
        BOOST_REQUIRE_CLOSE(output.fcn32.real().to_double(), 1.0, DBL_EPSILON);
        BOOST_REQUIRE_CLOSE(output.fcn32.imag().to_double(), 1.0, DBL_EPSILON);
        interp.tick();
    }

    for (size_t i = middleSectionBegin; i < middleSectionEnd; i++) {
        BOOST_CHECK(interp.output(output) == true);
        BOOST_REQUIRE_CLOSE(output.fcn32.real().to_double(), 0.0, DBL_EPSILON);
        BOOST_REQUIRE_CLOSE(output.fcn32.imag().to_double(), 0.0, DBL_EPSILON);
        interp.tick();
    }

    for (size_t i = rightSectionBegin; i < rightSectionEnd; i++) {
        BOOST_CHECK(interp.output(output) == true);
        BOOST_REQUIRE_CLOSE(output.fcn32.real().to_double(), -1.0, DBL_EPSILON);
        BOOST_REQUIRE_CLOSE(output.fcn32.imag().to_double(), -1.0, DBL_EPSILON);
        interp.tick();
    }

    //We should have drained the output by now
    BOOST_CHECK(interp.output(output) == false);
}

CSIM_TEST_CASE(INPUT_BUFFER_OVERFLOW)
{
    size_t chunkSize = 8;
    ZeroPadInterpolator interp(chunkSize);
    filter_io_t data, output;
    data = FixedComplex32(1.0, 1.0);

    for (size_t i = 0; i < chunkSize; i++) {
        BOOST_CHECK(interp.input(data) == true);
    }
    BOOST_CHECK(interp.input(data) == false);
}

CSIM_TEST_CASE(OUTPUT_BUFFER_OVERFLOW)
{
    size_t chunkSize = 8;
    ZeroPadInterpolator interp(chunkSize);
    filter_io_t data, output;
    data = FixedComplex32(1.0, 1.0);

    //Input twice chunk size to force input buffer to be flushed to output buffer and then fill up again
    for (size_t i = 0; i < chunkSize * 2; i++) {
        BOOST_CHECK(interp.input(data) == true);
        interp.tick();
    }
    BOOST_CHECK(interp.input(data) == false);

    //Check that we do start outputting here
    BOOST_CHECK(interp.output(output) == true);
}


CSIM_TEST_SUITE_END()
