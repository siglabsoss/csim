#include <test/unit_test.hpp>

#include <filters/subcarrier_mapper.hpp>
#include <filters/mapper.hpp>

CSIM_TEST_SUITE_BEGIN(validateSubcarrierMapper)

CSIM_TEST_CASE(OUTPUTS_PREAMBLE_BEFORE_SYMBOLS)
{
    //We're going to say that 124 carriers are inactive (62 on each side of the spectrum)
    std::vector<bool> subMask(1024, false);
    for (size_t i = (1024 - 900) / 2; i < 1024 - ((1024 - 900) / 2); i++) {
        subMask[i] = true;
    }
    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM16, 4000, 1024, subMask);
    SubcarrierMapper sm(mcs);

    constellation_map_t constellations = Mapper::getQAM16Constellations();

    //We're going to feed in [numActiveSubcarriers * numOFDMSymbols] samples. We're going to expect [numActiveSubcarriers * numOFDMSymbols] + 2*numSubcarriers samples of output
    //where 2*numSubcarriers is comprised of a short and long preamble
    size_t numInputSymbols = mcs.getNumActiveSubCarriers() * mcs.getNumOFDMSymbols();
    size_t totalPreambleLength = 2 * mcs.getNumSubCarriers();
    size_t numOutputs = 0;
    size_t numInputs = 0;
    filter_io_t sample;
    bool didStartOutput = false;
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
                //preamble outputs XXX make comparisons here
            } else {
                //symbol outputs
                BOOST_CHECK(constellations[0b0100] == sample.fc);
            }
            numOutputs++;
        } else {
            //we don't expect to not get an output if we've already started getting outputs
            BOOST_CHECK(didStartOutput == false);
        }
    }

    sm.tick();
    BOOST_CHECK(sm.output(sample) == false);
}

CSIM_TEST_SUITE_END()
