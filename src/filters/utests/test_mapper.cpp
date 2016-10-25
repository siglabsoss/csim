#include <test/unit_test.hpp>

#include <filters/mapper.hpp>
#include <filters/hard_demod.hpp>
#include <filters/soft_demod.hpp>
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
    uint8_t byte = 0b01010101;
    filter_io_t data, output;
    data = byte;
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);

    BOOST_CHECK_EQUAL(mod.input(data), true);
    for (unsigned int i = 0; i < (sizeof(byte) * 8) * MOD_TICKS_PER_SYMBOL; i++) {
        mod.tick();
        mod.output(output);
        int bit_idx = i / MOD_TICKS_PER_SYMBOL;
        if (byte & (1 << bit_idx)) {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(),  1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  0.0, DBL_EPSILON);
        } else {
            BOOST_CHECK_CLOSE(output.toComplexDouble().real(), -1.0, DBL_EPSILON);
            BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  0.0, DBL_EPSILON);
        }
    }
}


bool test_mod_hard_demod(Mapper& mod, HardDemod& demod, size_t ticks, uint8_t byte, uint8_t& byte_out)
{
//    cout << "testing " << (int) byte << endl;
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
//            cout << "worked " << worked2 << endl;
            if( worked2 )
            {
                byte_out = demod_output.byte;
                return true;
                cout << "out: " << (int)demod_output.byte << endl;
            }
        }
    }
    return false;
}

CSIM_TEST_CASE(MODULATOR_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
{
    uint8_t byte = 0;
    uint8_t byte_out = 0;
    bool worked0;
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    HardDemod demod(Mapper::CONST_SET_BPSK, 0);

    std::random_device engine;


    size_t ticks = sizeof(byte) * CHAR_BIT * MOD_TICKS_PER_SYMBOL;

    for(size_t i = 0; i < 300; i++)
    {

        if(i < 256)
        {
            byte = (uint8_t)i;
        }
        else
        {
            byte = engine();
        }

        worked0 = test_mod_hard_demod(mod, demod, ticks, byte, byte_out);

        BOOST_CHECK_EQUAL(worked0, true);
        BOOST_CHECK_EQUAL(byte, byte_out);
    }
}



bool test_mod_soft_demod(Mapper& mod, SoftDemod& demod, size_t ticks, uint8_t byte, uint8_t& byte_out)
{
    bool worked0, worked1, worked2;
    filter_io_t data, mod_output, demod_output;
    data = byte; // use the =operator overload
    worked0 = mod.input(data);
    BOOST_CHECK_EQUAL(worked0, true);
    size_t j = 0;
    byte_out = 0;
    double llr;
    for (size_t i = 0; i < ticks; i++) {
        mod.tick();
        worked1 = mod.output(mod_output);

        if( worked1 )
        {
            demod.input(mod_output);
            demod.tick();
            worked2 = demod.output(demod_output);

            if( worked2 )
            {
                llr = demod_output.fc.real().to_double();

                if( llr < 0 )
                {
                    byte_out |= (1<<j);
                }
                j++;
            }
        }
    }
    return j == 8;
}

CSIM_TEST_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
{
    uint8_t byte = 0;
    uint8_t byte_out = 0;
    bool worked0;
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    SoftDemod demod(Mapper::CONST_SET_BPSK);

    std::random_device engine;


    size_t ticks = sizeof(byte) * CHAR_BIT * MOD_TICKS_PER_SYMBOL;

    for(size_t i = 0; i < 300; i++)
    {

        if(i < 256)
        {
            byte = (uint8_t)i;
        }
        else
        {
            byte = engine();
        }

        worked0 = test_mod_soft_demod(mod, demod, ticks, byte, byte_out);


        BOOST_CHECK_EQUAL(worked0, true);
        BOOST_CHECK_EQUAL(byte, byte_out);
    }
}



CSIM_TESX_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_QPSK_SYMBOLS)
{
//    uint8_t byte = 0;
//    uint8_t byte_out = 0;
//    bool worked0;
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_QPSK);
    SoftDemod demod(Mapper::CONST_SET_QPSK);

    std::random_device engine;




    bool worked2;
    filter_io_t data, mod_output, demod_output, demod_input;
//    data = byte; // use the =operator overload
//    worked0 = mod.input(data);
//    BOOST_CHECK_EQUAL(worked0, true);

    //demod_input = ComplexDouble(-1,-1);

//    vector<ComplexDouble> inputs = {ComplexDouble(-M_SQRT1_2,-M_SQRT1_2), ComplexDouble(M_SQRT1_2,-M_SQRT1_2), ComplexDouble(-M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2/1.20,-M_SQRT1_2/1.20)};

    vector<ComplexDouble> inputs;

    vector<double> b1out;
    vector<double> fakellr;

    for(size_t i = 0; i < 100; i++)
    {
        double v =  -M_SQRT1_2 + (i*M_SQRT1_2/50.0);
        cout << v << endl;
        inputs.push_back(ComplexDouble(M_SQRT1_2, v));


        double pbi0 = (double)i;
        double pbi1 = (double)100 - i;
        fakellr.push_back(log2(pbi0/pbi1));
    }

    const plotter &plot = plotter::get();
    plot.nplotqam(inputs, "h");
//    plot.nplot(fakellr, "fake llr");

    for(auto it = inputs.begin(); it != inputs.end(); ++it)
    {
        demod_input = *it;
        demod.input(demod_input);
        demod.tick();
        worked2 = demod.output(demod_output);
        if(worked2) {
            cout << "input: " << demod_input.rf << endl << " output: " << std::real(demod_output.rf);


        }

        demod.tick();
        worked2 = demod.output(demod_output);
        if(worked2) {
            cout << " " << std::real(demod_output.rf) << endl;
            b1out.push_back(std::real(demod_output.rf));

        }
//        cout << endl << endl << endl << endl;
    }

    plot.nplot(b1out, "llr of b1");


//            cout << "worked " << worked2 << endl;
//    if( worked2 )
//    {
//        byte_out = demod_output.byte;
//        cout << "out: " << (int)demod_output.byte << endl;
//    }






//    size_t ticks = sizeof(byte) * CHAR_BIT * MOD_TICKS_PER_SYMBOL;
//
//    for(size_t i = 0; i < 300; i++)
//    {
//
//        if(i < 256)
//        {
//            byte = (uint8_t)i;
//        }
//        else
//        {
//            byte = engine();
//        }
//
//        worked0 = test_mod_demod(mod, demod, ticks, byte, byte_out);
//
//        BOOST_CHECK_EQUAL(worked0, true);
//        BOOST_CHECK_EQUAL(byte, byte_out);
//    }
}


CSIM_TESX_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
{
//    uint8_t byte = 0;
//    uint8_t byte_out = 0;
//    bool worked0;
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    SoftDemod demod(Mapper::CONST_SET_BPSK);

    std::random_device engine;




    bool worked2;
    filter_io_t data, mod_output, demod_output, demod_input;
//    data = byte; // use the =operator overload
//    worked0 = mod.input(data);
//    BOOST_CHECK_EQUAL(worked0, true);

    //demod_input = ComplexDouble(-1,-1);

//    vector<ComplexDouble> inputs = {ComplexDouble(-M_SQRT1_2,-M_SQRT1_2), ComplexDouble(M_SQRT1_2,-M_SQRT1_2), ComplexDouble(-M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2/1.20,-M_SQRT1_2/1.20)};

    vector<ComplexDouble> inputs;

    vector<double> b1out;
    vector<double> fakellr;

    double maxswing = 1.0; // this is M_SQRT1_2 for QPSK

    for(size_t i = 0; i < 100; i++)
    {
        double v =  -maxswing + (i*maxswing/50.0);
        cout << v << endl;
        inputs.push_back(ComplexDouble(v, 0));


        double pbi0 = (double)i;
        double pbi1 = (double)100 - i;
        fakellr.push_back(log2(pbi0/pbi1));
    }

    const plotter &plot = plotter::get();
    plot.nplotqam(inputs, "h");
//    plot.nplot(fakellr, "fake llr");

    for(auto it = inputs.begin(); it != inputs.end(); ++it)
    {
        demod_input = *it;
        demod.input(demod_input);
        demod.tick();
        worked2 = demod.output(demod_output);
        if(worked2) {
            cout << "input: " << demod_input.rf << endl << " output: " << std::real(demod_output.rf) << endl << endl;
            b1out.push_back(std::real(demod_output.rf));

        }

//        demod.tick();
//        worked2 = demod.output(demod_output);
//        if(worked2) {
//            cout << " " << std::real(demod_output.rf) << endl;
//
//
//        }
//        cout << endl << endl << endl << endl;
    }

    plot.nplot(b1out, "llr of b0");


//            cout << "worked " << worked2 << endl;
//    if( worked2 )
//    {
//        byte_out = demod_output.byte;
//        cout << "out: " << (int)demod_output.byte << endl;
//    }






//    size_t ticks = sizeof(byte) * CHAR_BIT * MOD_TICKS_PER_SYMBOL;
//
//    for(size_t i = 0; i < 300; i++)
//    {
//
//        if(i < 256)
//        {
//            byte = (uint8_t)i;
//        }
//        else
//        {
//            byte = engine();
//        }
//
//        worked0 = test_mod_demod(mod, demod, ticks, byte, byte_out);
//
//        BOOST_CHECK_EQUAL(worked0, true);
//        BOOST_CHECK_EQUAL(byte, byte_out);
//    }
}







CSIM_TESX_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS_XXX)
{
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
    SoftDemod demod(Mapper::CONST_SET_BPSK);

    std::random_device engine;




    bool worked2;
    filter_io_t data, mod_output, demod_output, demod_input;
//    data = byte; // use the =operator overload
//    worked0 = mod.input(data);
//    BOOST_CHECK_EQUAL(worked0, true);

    //demod_input = ComplexDouble(-1,-1);

//    vector<ComplexDouble> inputs = {ComplexDouble(-M_SQRT1_2,-M_SQRT1_2), ComplexDouble(M_SQRT1_2,-M_SQRT1_2), ComplexDouble(-M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2/1.20,-M_SQRT1_2/1.20)};

    vector<ComplexDouble> inputs;

    vector<double> b1out;
    vector<double> fakellr;

    double maxswing = 1.0; // this is M_SQRT1_2 for QPSK

    for(size_t i = 0; i < 100; i++)
    {
        double v =  -maxswing + (i*maxswing/50.0);
        cout << v << endl;
        inputs.push_back(ComplexDouble((i%2)*1.9-2, 0));


        double pbi0 = (double)i;
        double pbi1 = (double)100 - i;
        fakellr.push_back(log2(pbi0/pbi1));
    }

    const plotter &plot = plotter::get();
    plot.nplotqam(inputs, "h");
//    plot.nplot(fakellr, "fake llr");

    for(auto it = inputs.begin(); it != inputs.end(); ++it)
    {
        demod_input = *it;
        demod.input(demod_input);
        demod.tick();
        worked2 = demod.output(demod_output);
        if(worked2) {
            cout << "input: " << demod_input.rf << endl << " output: " << std::real(demod_output.rf) << endl << endl;
            b1out.push_back(std::real(demod_output.rf));

        }

//        demod.tick();
//        worked2 = demod.output(demod_output);
//        if(worked2) {
//            cout << " " << std::real(demod_output.rf) << endl;
//
//
//        }
//        cout << endl << endl << endl << endl;
    }

    plot.nplot(b1out, "llr of b0");


}







CSIM_TEST_CASE(MODULATOR_DOES_OUTPUT_CORRECT_QAM16_SYMBOLS)
{
    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_QAM16);
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

    constellation_map_t expectedConstellations =  Mapper::getQAM16Constellations();

    size_t expectedTicksPerOutput = 2; //derived from parameters at top of file
    size_t symbolsPerByte = 2;
    for (unsigned int i = 0; i < sizeof(testData) * symbolsPerByte * expectedTicksPerOutput; i++) {
        mod.tick();
        mod.output(output);
        int symbol_idx = i / MOD_TICKS_PER_SYMBOL;
        constellation_t current = expectedConstellations[symbol_idx];
        BOOST_CHECK_CLOSE(output.toComplexDouble().real(),  current.real().to_double(), DBL_EPSILON);
        BOOST_CHECK_CLOSE(output.toComplexDouble().imag(),  current.imag().to_double(), DBL_EPSILON);
    }
}

CSIM_TEST_SUITE_END()
