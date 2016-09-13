#include <test/unit_test.hpp>

#include <filters/modulator.hpp>
#include <filters/hard_demod.hpp>
#include <boost/integer/integer_mask.hpp>

#include <cfloat>

static constexpr size_t MOD_TICKS_PER_SYMBOL = 3;



CSIM_TEST_SUITE_BEGIN(ModulatorFunctionality)

CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_NULL_SYMBOL)
{
    Modulator mod(MOD_TICKS_PER_SYMBOL, Modulator::MOD_SCHEME_BPSK);
    filter_io_t output;
    for (int i = 0; i < 100; i++) {
        mod.tick();
        mod.output(output);
        BOOST_CHECK_CLOSE(output.fc.real().to_double(),  0.0, DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.fc.imag().to_double(),  0.0, DBL_EPSILON);
    }
}

CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_CORRECT_BPSK_SYMBOLS)
{
    uint8_t byte = 0b01010101;
    filter_io_t data, output;
    data = byte;
    Modulator mod(MOD_TICKS_PER_SYMBOL, Modulator::MOD_SCHEME_BPSK);

    BOOST_CHECK_EQUAL(mod.input(data), true);
    for (unsigned int i = 0; i < (sizeof(byte) * 8) * MOD_TICKS_PER_SYMBOL; i++) {
        mod.tick();
        mod.output(output);
        int bit_idx = i / MOD_TICKS_PER_SYMBOL;
        if (byte & (1 << bit_idx)) {
            BOOST_CHECK_CLOSE(output.fc.real().to_double(),  1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.fc.imag().to_double(),  0.0, DBL_EPSILON);
        } else {
            BOOST_CHECK_CLOSE(output.fc.real().to_double(), -1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.fc.imag().to_double(),  0.0, DBL_EPSILON);
        }
    }
}


bool test_mod_demod(Modulator& mod, HardDemod& demod, size_t ticks, uint8_t byte, uint8_t& byte_out)
{
    bool worked0, worked1, worked2;
    filter_io_t data, mod_output, demod_output;
    data = byte; // use the =operator overload
    worked0 = mod.input(data);
    BOOST_CHECK_EQUAL(worked0, true);
    for (size_t i = 0; i < ticks; i++) {
        mod.tick();
        worked1 = mod.output(mod_output);

        if( worked1 )
        {


            demod.input(mod_output);
            demod.tick();
            worked2 = demod.output(demod_output);
            cout << "worked " << worked2 << endl;
            if( worked2 )
            {
                byte_out = demod_output.byte;
                cout << "out: " << (int)demod_output.byte << endl;
            }
        }

        int bit_idx = i / MOD_TICKS_PER_SYMBOL;

        cout << "bit_idx " << bit_idx << endl;
    }
}

CSIM_TEST_CASE(MODULATOR_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
{
    uint8_t byte = 0b01010101;
    __attribute__ ((unused)) uint8_t byte_out = 0;
    filter_io_t data, mod_output, demod_output;
    data = byte;
    bool worked0, worked1, worked2;
    Modulator mod(MOD_TICKS_PER_SYMBOL, Modulator::MOD_SCHEME_BPSK);
    HardDemod demod(Modulator::MOD_SCHEME_BPSK, 0);


    worked0 = mod.input(data);
    BOOST_CHECK_EQUAL(worked0, true);
    for (unsigned int i = 0; i < (sizeof(byte) * 8) * MOD_TICKS_PER_SYMBOL; i++) {
        mod.tick();
        worked1 = mod.output(mod_output);

        if( worked1 )
        {
//            cout << "rf: " << mod_output.fc << endl;


            demod.input(mod_output);
            demod.tick();
            worked2 = demod.output(demod_output);
            cout << "worked " << worked2 << endl;
            if( worked2 )
            {
                byte_out = demod_output.byte;
                cout << "out: " << (int)demod_output.byte << endl;
            }
        }

        int bit_idx = i / MOD_TICKS_PER_SYMBOL;

        cout << "bit_idx " << bit_idx << endl;
//        if (byte & (1 << bit_idx)) {
//            BOOST_CHECK_CLOSE(output.fc.real().to_double(),  1.0, DBL_EPSILON);
//            BOOST_CHECK_CLOSE(output.fc.imag().to_double(),  0.0, DBL_EPSILON);
//        } else {
//            BOOST_CHECK_CLOSE(output.fc.real().to_double(), -1.0, DBL_EPSILON);
//            BOOST_CHECK_CLOSE(output.fc.imag().to_double(),  0.0, DBL_EPSILON);
//        }
    }
}


CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_CORRECT_QAM16_SYMBOLS)
{
    Modulator mod(MOD_TICKS_PER_SYMBOL, Modulator::MOD_SCHEME_QAM16);
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
    for (unsigned int i = 0; i < sizeof(testData); i++) {
        data = testData[i];
        BOOST_CHECK_EQUAL(mod.input(data), true);
    }

    constellation_map_t expectedConstellations =  Modulator::getQAM16Constellations();

    size_t expectedTicksPerOutput = 2; //derived from parameters at top of file
    size_t symbolsPerByte = 2;
    for (unsigned int i = 0; i < sizeof(testData) * symbolsPerByte * expectedTicksPerOutput; i++) {
        mod.tick();
        mod.output(output);
        int symbol_idx = i / MOD_TICKS_PER_SYMBOL;
        constellation_t current = expectedConstellations[symbol_idx];
        BOOST_CHECK_CLOSE(output.fc.real().to_double(),  current.real().to_double(), DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.fc.imag().to_double(),  current.imag().to_double(), DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
