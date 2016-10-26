#include <test/unit_test.hpp>

#include <filters/mapper.hpp>
#include <utils/plotter.hpp>


#include <cfloat>

static constexpr size_t MOD_TICKS_PER_SYMBOL = 3;

using namespace std;

CSIM_TEST_SUITE_BEGIN(MapperFunctionality)


CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_NULL_SYMBOL)
{
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
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
    std::vector<bool> byte = {0,1,0,1,0,1,0,1};
    filter_io_t data, output;
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    for (size_t i = 0; i < byte.size(); i++) {
        data.type = IO_TYPE_BIT;
        data.bit = byte[i];
        BOOST_CHECK_EQUAL(mod.input(data), true);
    }
    for (unsigned int i = 0; i < (8) * MOD_TICKS_PER_SYMBOL; i++) {
        mod.tick();
        mod.output(output);
        int bit_idx = i / MOD_TICKS_PER_SYMBOL;
        if (byte[bit_idx]) {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(),  1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  0.0, DBL_EPSILON);
        } else {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(), -1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  0.0, DBL_EPSILON);
        }
    }
}


CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_CORRECT_QAM16_SYMBOLS)
{
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_QAM16);

    std::vector<bool> testData = {
            0,0,0,1,
            0,0,0,0,
            0,0,1,1,
            0,0,1,0,
            0,1,0,1,
            0,1,0,0,
            0,1,1,1,
            0,1,1,0,
            1,0,0,1,
            1,0,0,0,
            1,0,1,1,
            1,0,1,0,
            1,1,0,1,
            1,1,0,0,
            1,1,1,1,
            1,1,1,0};
    std::vector<int> symbols = {1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14};
    filter_io_t data, output;
    for (unsigned int i = 0; i < testData.size(); i++) {
        data.type = IO_TYPE_BIT;
        data.bit  = testData[testData.size() - 1 - i];
        BOOST_CHECK_EQUAL(mod.input(data), true);
    }

    constellation_map_t expectedConstellations =  Mapper::getQAM16Constellations();

    size_t expectedTicksPerOutput = 2; //derived from parameters at top of file
    size_t symbolsPerByte = 2;
    for (unsigned int i = 0; i < symbolsPerByte * expectedTicksPerOutput; i++) {
        mod.tick();
        mod.output(output);


        int symbol_idx = i / MOD_TICKS_PER_SYMBOL;
        symbol_idx = 15 - symbol_idx;
        int symbol = symbols[symbol_idx];
        constellation_t current = expectedConstellations[symbol];
        BOOST_CHECK_CLOSE(output.toComplexDouble().real(),  current.real().to_double(), DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  current.imag().to_double(), DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
