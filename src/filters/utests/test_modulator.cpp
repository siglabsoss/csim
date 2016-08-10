#include <test/unit_test.hpp>

#include <filters/modulator.hpp>
#include <core/parameters.hpp>

#include <cfloat>

//parameters defined such that modulator sample rate is half so ticks per sample is 2
PARAM_DEFINE(RADIO_DIGITAL_SAMPLERATE, 500000l);
PARAM_DEFINE(MOD_SAMPLERATE, 250000l);

CSIM_TEST_SUITE_BEGIN(ModulatorFunctionality)

CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_ZERO_BIT_SYMBOL)
{
    Modulator mod(Modulator::MOD_SCHEME_BPSK);

    for (int i = 0; i < 100; i++) {
        filter_io_t output;
        mod.tick();
        mod.output(output);
        BOOST_CHECK_CLOSE(output.fcn32.real().to_double(), -1.0, DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(),  0.0, DBL_EPSILON);
    }
}

CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_CORRECT_BPSK_SYMBOLS)
{
    uint8_t byte = 0b01010101;
    filter_io_t data, output;
    data = byte;
    Modulator mod(Modulator::MOD_SCHEME_BPSK);

    BOOST_CHECK_EQUAL(mod.input(data), true);
    size_t expectedTicksPerOutput = 2; //derived from parameters at top of file
    for (int i = 0; i < sizeof(byte) * expectedTicksPerOutput; i++) {
        mod.tick();
        mod.output(output);
        int bit_idx = i / expectedTicksPerOutput;
        if (byte & (1 << bit_idx)) {
            BOOST_CHECK_CLOSE(output.fcn32.real().to_double(),  1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(),  0.0, DBL_EPSILON);
        } else {
            BOOST_CHECK_CLOSE(output.fcn32.real().to_double(), -1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(),  0.0, DBL_EPSILON);
        }
    }
}

CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_CORRECT_QAM16_SYMBOLS)
{
    Modulator mod(Modulator::MOD_SCHEME_QAM16);
    uint8_t testData[] = {
            0b00010000,
            0b00110010,
            0b01010100,
            0b01110110,
            0b10011000,
            0b10111010,
            0b11011100,
            0b11111110
    };
    filter_io_t data, output;
    for (int i = 0; i < sizeof(testData); i++) {
        data = testData[i];
        BOOST_CHECK_EQUAL(mod.input(data), true);
    }

    constellation_map_t expectedConstellations =  Modulator::getQAM16Constellations();

    size_t expectedTicksPerOutput = 2; //derived from parameters at top of file
    size_t symbolsPerByte = 2;
    for (int i = 0; i < sizeof(testData) * symbolsPerByte * expectedTicksPerOutput; i++) {
        mod.tick();
        mod.output(output);
        int symbol_idx = i / expectedTicksPerOutput;
        constellation_t current = expectedConstellations[symbol_idx];
        BOOST_CHECK_CLOSE(output.fcn32.real().to_double(),  current.real().to_double(), DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.fcn32.imag().to_double(),  current.imag().to_double(), DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
