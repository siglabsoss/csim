#include <test/unit_test.hpp>

#define private public
#include <filters/subcarrier_mapper.hpp>
#undef private

#include <filters/mapper.hpp>

static std::vector<ComplexDouble>generatedExpectedPreamble(
    const SubcarrierMapper& sm)
{
    std::vector<ComplexDouble> expectedPreambleTones(1024);

    // The preamble symbols are generated using this PRNG with the default seed.
    // Using the same
    // generator here so that we generate the same sequence for testing.
    // Exploiting this implementation
    // detail is not ideal, but it's OK for now because we're likely to change
    // the PN sequence for the
    // preamble. This is to move us forward for now.
    auto g =
        std::bind(std::uniform_int_distribution<unsigned>(0, 1), std::mt19937());
    size_t numPilots  = sm.getNumPilotTones();
    double powerScale = sqrt(expectedPreambleTones.size() / numPilots);

    for (size_t n = 0; n < expectedPreambleTones.size() / 2;
         n += 2) {
        // Positive tone
        size_t fftIdx = sm.subcarrierIdxToFFTIdx(n);
        int    val    = (g() << 1) - 1;
        expectedPreambleTones[fftIdx].real(static_cast<double>(val) * powerScale);

        // Corresponding negative tone
        fftIdx = sm.subcarrierIdxToFFTIdx(-n);
        val    = (g() << 1) - 1;
        expectedPreambleTones[fftIdx].real(static_cast<double>(val) * powerScale);
    }

    return expectedPreambleTones;
}

CSIM_TEST_SUITE_BEGIN(validateSubcarrierMapper)

CSIM_TEST_CASE(OUTPUTS_PREAMBLE_BEFORE_SYMBOLS)
{
    // We're going to say that 124 carriers are inactive (62 on each side of the
    // spectrum). This will determine how many output samples to expect after
    // the preamble
    std::vector<bool> subMask(1024, false);

    for (size_t i = (1024 - 900) / 2; i < 1024 - ((1024 - 900) / 2); i++) {
        subMask[i] = true;
    }

    size_t numPilotSymbols = 30;

    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM16, 4000, 1024, subMask);
    SubcarrierMapper sm(mcs, 2 /* pilot spacing */,
                        0 /* initial pilot offset */,
                        numPilotSymbols /* number of repetitions */);

    constellation_map_t constellations = Mapper::getQAM16Constellations();

    // We're going to feed in [numActiveSubcarriers * numOFDMSymbols] samples.
    // We're going to expect [numActiveSubcarriers * numOFDMSymbols] +
    // 2*numSubcarriers samples of output
    // where 2*numSubcarriers is comprised of a short and long preamble
    const size_t numInputSymbols = mcs.getNumActiveSubCarriers() *
                                   mcs.getNumOFDMSymbols();

    // Both preambles are as long as the number of subcarriers each.
    const size_t totalPreambleLength = numPilotSymbols * mcs.getNumSubCarriers();
    filter_io_t  sample;

    std::vector<ComplexDouble> expectedPreambleTones = generatedExpectedPreamble(
        sm);

    for (size_t i = 0; i < 4; ++i) {
        size_t numOutputs     = 0;
        size_t numInputs      = 0;
        bool   didStartOutput = false;

        while (numOutputs < totalPreambleLength + numInputSymbols) {
            sample = constellations[0b0100];

            if (numInputs < numInputSymbols) {
                BOOST_CHECK(sm.input(sample) == true);
                numInputs++;
            }
            sm.tick();

            if (sm.output(sample)) {
                didStartOutput = true;
                BOOST_CHECK(sample.type == IO_TYPE_COMPLEX_FIXPOINT);

                if (numOutputs < totalPreambleLength) {
                    BOOST_CHECK_CLOSE(
                        sample.fc.real().to_double(),
                        expectedPreambleTones[numOutputs %
                                              expectedPreambleTones.size()].real(),
                        1.0 / (1 << 10));
                    BOOST_CHECK_CLOSE(
                        sample.fc.imag().to_double(),
                        expectedPreambleTones[numOutputs %
                                              expectedPreambleTones.size()].imag(),
                        1.0 / (1 << 10));
                } else {
                    // symbol outputs
                    BOOST_CHECK(constellations[0b0100] == sample.fc);
                }
                numOutputs++;
            } else {
                // we don't expect to miss an output if we've already started
                // getting outputs
                    BOOST_CHECK(didStartOutput == false);
            }
        }
    }

    // We expect no output now that we've output the exact number we expected.
    sm.tick();
                    BOOST_CHECK(sm.output(sample) == false);
}

CSIM_TEST_SUITE_END()
