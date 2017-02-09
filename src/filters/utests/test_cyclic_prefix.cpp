#include <test/unit_test.hpp>

#include <filters/cyclic_prefix.hpp>

CSIM_TEST_SUITE_BEGIN(CyclicPrefixVerification)

static void runCyclicPrefixValidation(size_t Nfft,
                                      size_t Ncp,
                                      size_t ticksPerOutput)
{
    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, 4096, 1024);
    CyclicPrefix cp(Nfft, Ncp, ticksPerOutput, mcs);

    std::vector<SLFixComplex> inputs(Nfft, SLFixComplex(
                                         18,
                                         6,
                                         SLFixPoint::
                                         QUANT_RND_HALF_UP,
                                         SLFixPoint::
                                         OVERFLOW_SATURATE));

    // Generate inputs
    for (size_t i = 0; i < inputs.size(); i++) {
        inputs[i] = static_cast<double>(i); // give each input a unique value
    }

    // Load up inputs
    for (size_t i = 0; i < inputs.size(); i++) {
        filter_io_t data;
        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(inputs[i].getFormat());
        data.fc = inputs[i];
        BOOST_REQUIRE(cp.input(data) == true);
        cp.tick();
        BOOST_REQUIRE(cp.output(data) == (i % 10 == 0));
    }

    // Call through until we're about to output again
    for (size_t i = 0; i < ticksPerOutput - (inputs.size() % ticksPerOutput) + 1;
         i++) {
        filter_io_t data;
        BOOST_CHECK(cp.output(data) == false);
        cp.tick();
    }

    // Call through until we've gotten enough outputs (Nfft + Ncp)
    std::vector<SLFixComplex> outputs(Nfft + Ncp);

    for (size_t i = 0; i < (outputs.size() * ticksPerOutput); i++) {
        filter_io_t data;
        bool expectOutput = (i % ticksPerOutput) == 0;
        BOOST_REQUIRE_MESSAGE(cp.output(
                                  data) == expectOutput,
                              "Did not get output when expected or vice versa on iteration number "
                              << i);

        if (expectOutput) {
            BOOST_REQUIRE(data.type == IO_TYPE_COMPLEX_FIXPOINT);
            size_t outIdx = i / ticksPerOutput;
            outputs[outIdx].setFormat(data.fc.getFormat());
            outputs[outIdx] = data.fc;
        }
        cp.tick();
    }

    // Confirm that output is input with a cyclic prefix prepended
    size_t inputIdx = (inputs.size() - Ncp) % inputs.size();

    for (size_t i = 0; i < outputs.size(); i++) {
        BOOST_CHECK_EQUAL(outputs[i].real().to_int64(),
                          inputs[inputIdx].real().to_int64());
        BOOST_CHECK_EQUAL(outputs[i].imag().to_int64(),
                          inputs[inputIdx].imag().to_int64());
        inputIdx = (inputIdx + 1) % inputs.size(); // wrap around automatically
    }

    // Check for several more outputs. They should come at the same frequency,
    // but be zero
    for (size_t i = 0; i < 10 * ticksPerOutput; i++) {
        filter_io_t data;
        bool expectOutput = (i % ticksPerOutput) == 0;
        BOOST_REQUIRE_MESSAGE(cp.output(
                                  data) == expectOutput,
                              "Did not get output when expected or vice versa on iteration number "
                              << i);

        if (expectOutput) {
            BOOST_REQUIRE(data.type == IO_TYPE_COMPLEX_FIXPOINT);
            BOOST_CHECK_EQUAL(data.fc.real().to_int64(), 0);
            BOOST_CHECK_EQUAL(data.fc.imag().to_int64(), 0);
        }
        cp.tick();
    }
}

CSIM_TEST_CASE(CYCLIC_PREFIX_IS_APPENDED)
{
    runCyclicPrefixValidation(32, 8, 10);
}

CSIM_TEST_CASE(CYCLIC_PREFIX_0_LENGTH)
{
    runCyclicPrefixValidation(32, 0, 10);
}

CSIM_TEST_SUITE_END()
