#include <test/unit_test.hpp>

#include <filters/cyclic_prefix.hpp>

CSIM_TEST_SUITE_BEGIN(CyclicPrefixVerification)

CSIM_TEST_CASE(CYCLIC_PREFIX_IS_APPENDED)
{
    constexpr size_t Nfft = 32;
    constexpr size_t Ncp  = 8;
    constexpr size_t TICKS_PER_OUTPUT = 10;
    CyclicPrefix cp(Nfft, Ncp, TICKS_PER_OUTPUT);

    std::vector< SLFixComplex> inputs(Nfft, SLFixComplex(18, 6, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));

    //Generate inputs
    for (size_t i = 0; i < inputs.size(); i++) {
        inputs[i] = static_cast<double>(i); //give each input a unique value
    }

    //Load up inputs
    for (size_t i = 0; i < inputs.size(); i++) {
        filter_io_t data;
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(inputs[i]);
        data.fc = inputs[i];
        BOOST_REQUIRE(cp.input(data) == true);
        cp.tick();
        BOOST_REQUIRE(cp.output(data) == (i % 10 == 0));
    }

    //Call through until we're about to output again
    for (size_t i = 0; i < TICKS_PER_OUTPUT - (inputs.size() % TICKS_PER_OUTPUT) + 1; i++) {
        filter_io_t data;
        BOOST_CHECK(cp.output(data) == false);
        cp.tick();
    }

    //Call through until we've gotten enough outputs (Nfft + Ncp)
    std::vector< SLFixComplex > outputs(Nfft + Ncp);
    for (size_t i = 0; i < (outputs.size() * TICKS_PER_OUTPUT); i++) {
        filter_io_t data;
        bool expectOutput = (i % TICKS_PER_OUTPUT) == 0;
        BOOST_REQUIRE_MESSAGE(cp.output(data) == expectOutput, "Did not get output when expected or vice versa on iteration number " << i);
        if (expectOutput) {
            BOOST_REQUIRE(data.type == IO_TYPE_COMPLEX_FIXPOINT);
            size_t outIdx = i / TICKS_PER_OUTPUT;
            outputs[outIdx].setFormat(data.fc);
            outputs[outIdx] = data.fc;
        }
        cp.tick();
    }

    //Confirm that output is input with a cyclic prefix prepended
    size_t inputIdx = inputs.size() - Ncp;
    for (size_t i = 0; i < outputs.size(); i++) {
        BOOST_CHECK_EQUAL(outputs[i].real().to_int64(), inputs[inputIdx].real().to_int64());
        BOOST_CHECK_EQUAL(outputs[i].imag().to_int64(), inputs[inputIdx].imag().to_int64());
        inputIdx = (inputIdx + 1) % inputs.size(); //wrap around automatically
    }

    //Check for several more outputs. They should come at the same frequency, but be zero
    for (size_t i = 0; i < 10 * TICKS_PER_OUTPUT; i++) {
        filter_io_t data;
        bool expectOutput = (i % TICKS_PER_OUTPUT) == 0;
        BOOST_REQUIRE_MESSAGE(cp.output(data) == expectOutput, "Did not get output when expected or vice versa on iteration number " << i);
        if (expectOutput) {
            BOOST_REQUIRE(data.type == IO_TYPE_COMPLEX_FIXPOINT);
            BOOST_CHECK_EQUAL(data.fc.real().to_int64(), 0);
            BOOST_CHECK_EQUAL(data.fc.imag().to_int64(), 0);
        }
        cp.tick();
    }
}

CSIM_TEST_SUITE_END()
