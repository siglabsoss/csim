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

    //Both preambles are as long as the number of subcarriers each.
    size_t totalPreambleLength = 2 * mcs.getNumSubCarriers();
    size_t numOutputs = 0;
    size_t numInputs = 0;
    filter_io_t sample;
    bool didStartOutput = false;

    //The preamble symbols are generated using this PRNG with the default seed. Using the same
    //generator here so that we generate the same sequence for testing. Exploiting this implementation
    //detail is not ideal, but it's OK for now because we're likely to change the PN sequence for the
    //preamble. This is to move us forward for now.
    auto g = std::bind(std::uniform_int_distribution<unsigned>(0,1), std::mt19937());
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
                if (numOutputs < totalPreambleLength/2) {
                    //The short preamble repeats four times in time within an OFDM symbol. This is because we loaded every
                    //fourth subchannel with a non-zero value
                    if (numOutputs % 4 == 0) {
                        int val = (g() << 1) - 1;
                        BOOST_CHECK_EQUAL(sample.fc.real().to_double(), static_cast<double>(val));
                        BOOST_CHECK_EQUAL(sample.fc.imag().m_value, 0);
                    } else {
                        BOOST_CHECK_EQUAL(sample.fc.real().m_value, 0);
                        BOOST_CHECK_EQUAL(sample.fc.imag().m_value, 0);
                    }
                } else {
                    //The long preamble repeats twice in time within an OFDM symbol. This is because we loaded every
                    //second subchannel with a non-zero value
                    if (numOutputs % 2 == 0) {
                        int val = (g() << 1) - 1;
                        BOOST_CHECK_EQUAL(sample.fc.real().to_double(), static_cast<double>(val));
                        BOOST_CHECK_EQUAL(sample.fc.imag().m_value, 0);
                    } else {
                        BOOST_CHECK_EQUAL(sample.fc.real().m_value, 0);
                        BOOST_CHECK_EQUAL(sample.fc.imag().m_value, 0);
                    }
                }
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

    //We expect no output now that we've output the exact number we expected.
    sm.tick();
    BOOST_CHECK(sm.output(sample) == false);
}

CSIM_TEST_SUITE_END()