#include <test/unit_test.hpp>

#include <filters/mapper.hpp>
#include <utils/plotter.hpp>


#include <cfloat>

static constexpr size_t MAPPER_TICKS_PER_SYMBOL = 3;

using namespace std;

CSIM_TEST_SUITE_BEGIN(MapperFunctionality)


CSIM_TEST_CASE(MAPPER_DOES_NOT_OUTPUT_BEFORE_FIRST_SYMBOL)
{
    Mapper mapper(MAPPER_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    filter_io_t output;
    for (int i = 0; i < 100; i++) {
        mapper.tick();
        BOOST_CHECK(!mapper.output(output));
    }
}

CSIM_TEST_CASE(MAPPER_DOES_OUTPUT_CORRECT_BPSK_SYMBOLS)
{
    std::vector<bool> byte = {0,1,0,1,0,1,0,1};
    filter_io_t data, output;
    Mapper mapper(MAPPER_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    for (size_t i = 0; i < byte.size(); i++) {
        data.type = IO_TYPE_BIT;
        data.bit = byte[i];
        BOOST_CHECK_EQUAL(mapper.input(data), true);
    }

    //The mapper will scramble the inputs, so we scramble them as well here
    Scrambler scr(0b1111111);
    scr.scramble(byte);

    for (unsigned int i = 0; i < (8) * MAPPER_TICKS_PER_SYMBOL; i++) {
        mapper.tick();
        mapper.output(output);
        int bit_idx = i / MAPPER_TICKS_PER_SYMBOL;
        if (byte[bit_idx]) {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(),  1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  0.0, DBL_EPSILON);
        } else {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(), -1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  0.0, DBL_EPSILON);
        }
    }
}

CSIM_TEST_CASE(MAPPER_DOES_OUTPUT_CORRECT_QAM16_SYMBOLS)
{
    Mapper mapper(MAPPER_TICKS_PER_SYMBOL, Mapper::CONST_SET_QAM16);

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

    filter_io_t data, output;
    for (unsigned int i = 0; i < testData.size(); i++) {
        data.type = IO_TYPE_BIT;
        data.bit  = testData[testData.size() - 1 - i];
        BOOST_CHECK_EQUAL(mapper.input(data), true);
    }

    constellation_map_t expectedConstellations =  Mapper::getQAM16Constellations();

    //These are the expected symbols for the test data.
    std::vector<int> symbols = {2,0,5,15,2,0,1,2,13,8,2,9,9,3,8,14};

    for (unsigned int i = 0; i < testData.size() / 4 * MAPPER_TICKS_PER_SYMBOL; i++) {
        mapper.tick();
        mapper.output(output);

        int symbol_idx = 15 - (i / MAPPER_TICKS_PER_SYMBOL);
        int symbol = symbols[symbol_idx];
        constellation_t current = expectedConstellations[symbol];
        BOOST_CHECK_CLOSE(output.toComplexDouble().real(),  current.real().to_double(), DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  current.imag().to_double(), DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
