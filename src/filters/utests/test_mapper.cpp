#include <test/unit_test.hpp>

#include <filters/mapper.hpp>
#include <utils/plotter.hpp>


#include <cfloat>

using namespace std;

CSIM_TEST_SUITE_BEGIN(MapperFunctionality)


CSIM_TEST_CASE(MAPPER_DOES_NOT_OUTPUT_BEFORE_FIRST_SYMBOL)
{
    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_BPSK, 0);
    Mapper mapper(mcs);
    filter_io_t output;

    for (int i = 0; i < 100; i++) {
        mapper.tick();
        BOOST_CHECK(!mapper.output(output));
    }
}

CSIM_TEST_CASE(MAPPER_DOES_OUTPUT_CORRECT_BPSK_SYMBOLS)
{
    std::vector<bool> byte = { 0, 1, 0, 1, 0, 1, 0, 1 };
    filter_io_t data, output;
    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_BPSK, 0);
    Mapper mapper(mcs);

    for (size_t i = 0; i < byte.size(); i++) {
        data.type = IO_TYPE_BIT;
        data.bit  = byte[i];
        BOOST_CHECK_EQUAL(mapper.input(data), true);
    }

    for (unsigned int i = 0; i < 8; i++) {
        mapper.tick();
        mapper.output(output);
        int bit_idx = i;

        if (byte[bit_idx]) {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(), 1.0,  DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(), 0.0,  DBL_EPSILON);
        } else {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(), -1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(), 0.0,  DBL_EPSILON);
        }
    }
}

CSIM_TEST_CASE(MAPPER_DOES_OUTPUT_CORRECT_QAM16_SYMBOLS)
{
    MCS mcs(MCS::FIVE_SIXTHS_RATE, MCS::MOD_QAM16, 0, 104, std::vector<bool>(104,
                                                                             true));
    Mapper mapper(mcs);

    std::vector<bool> testData = {
        0, 0, 0, 1,
        0, 0, 0, 0,
        0, 0, 1, 1,
        0, 0, 1, 0,
        0, 1, 0, 1,
        0, 1, 0, 0,
        0, 1, 1, 1,
        0, 1, 1, 0,
        1, 0, 0, 1,
        1, 0, 0, 0,
        1, 0, 1, 1,
        1, 0, 1, 0,
        1, 1, 0, 1,
        1, 1, 0, 0,
        1, 1, 1, 1,
        1, 1, 1, 0
    };

    filter_io_t data, output;

    for (unsigned int i = 0; i < testData.size(); i++) {
        data.type = IO_TYPE_BIT;
        data.bit  = testData[testData.size() - 1 - i];
        BOOST_CHECK_EQUAL(mapper.input(data), true);
    }

    constellation_map_t expectedConstellations =  Mapper::getQAM16Constellations();

    // These are the expected symbols for the test data.
    std::vector<int> symbols =
    { 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };

    for (unsigned int i = 0; i < testData.size() / 4; i++) {
        mapper.tick();
        mapper.output(output);

        int symbol_idx          = 15 - (i);
        int symbol              = symbols[symbol_idx];
        constellation_t current = expectedConstellations[symbol];
        BOOST_CHECK_CLOSE(output.toComplexDouble().real(),
                          current.real().to_double(), DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),
                          current.imag().to_double(), DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
