#include <test/unit_test.hpp>

#define private public
#include <filters/demapper.hpp>
#undef private
#include <utils/plotter.hpp>


#include <cfloat>
using namespace std;

CSIM_TEST_SUITE_BEGIN(DemapperFunctionality)

CSIM_TEST_CASE(LLR_CALCULATION)
{
    //For a BPSK symbol received, the LLR is log(probability bit = 0 / probability bit = 1)
    //equation is here: http://itpp.sourceforge.net/4.3.1/form_149.png
    //positive LLR is more likely 0, negative LLR is more likely 1

    //The example input/output was generated from a Simulink simulation (-0.9125, 0.0) -> 3.65 LLR
    double awgnVar = 1.0;
    double p0   = Demapper::calcLLRIncrement(ComplexDouble(-0.9125, 0.0), ComplexDouble(-1.0, 0.0), awgnVar);
    double p1   = Demapper::calcLLRIncrement(ComplexDouble(-0.9125, 0.0), ComplexDouble( 1.0, 0.0), awgnVar);

    BOOST_CHECK_CLOSE(log(p0/p1), 3.65, 0.001);
}

//bool test_mod_hard_demod(Mapper& mod, HardDemod& demod, size_t ticks, uint8_t byte, uint8_t& byte_out)
//{
////    cout << "testing " << (int) byte << endl;
//    bool worked0, worked1, worked2;
//    filter_io_t data, mod_output, demod_output;
//    data = byte; // use the =operator overload
//    worked0 = mod.input(data);
//    BOOST_CHECK_EQUAL(worked0, true);
//    for (size_t i = 0; i < ticks; i++) {
//        mod.tick();
//        worked1 = mod.output(mod_output);
//
//        if( worked1 )
//        {
//            demod.input(mod_output);
//            demod.tick();
//            worked2 = demod.output(demod_output);
////            cout << "worked " << worked2 << endl;
//            if( worked2 )
//            {
//                byte_out = demod_output.byte;
//                return true;
//                cout << "out: " << (int)demod_output.byte << endl;
//            }
//        }
//    }
//    return false;
//}
//
//CSIM_TEST_CASE(MODULATOR_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
//{
//    uint8_t byte = 0;
//    uint8_t byte_out = 0;
//    bool worked0;
//    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
//    HardDemod demod(Mapper::CONST_SET_BPSK, 0);
//
//    std::random_device engine;
//
//
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
//        worked0 = test_mod_hard_demod(mod, demod, ticks, byte, byte_out);
//
//        BOOST_CHECK_EQUAL(worked0, true);
//        BOOST_CHECK_EQUAL(byte, byte_out);
//    }
//}
//
//
//
//bool test_mod_soft_demod(Mapper& mod, SoftDemod& demod, size_t ticks, uint8_t byte, uint8_t& byte_out)
//{
//    bool worked0, worked1, worked2;
//    filter_io_t data, mod_output, demod_output;
//    data = byte; // use the =operator overload
//    worked0 = mod.input(data);
//    BOOST_CHECK_EQUAL(worked0, true);
//    size_t j = 0;
//    byte_out = 0;
//    double llr;
//    for (size_t i = 0; i < ticks; i++) {
//        mod.tick();
//        worked1 = mod.output(mod_output);
//
//        if( worked1 )
//        {
//            demod.input(mod_output);
//            demod.tick();
//            worked2 = demod.output(demod_output);
//
//            if( worked2 )
//            {
//                llr = demod_output.fc.real().to_double();
//
//                if( llr < 0 )
//                {
//                    byte_out |= (1<<j);
//                }
//                j++;
//            }
//        }
//    }
//    return j == 8;
//}
//
//CSIM_TEST_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
//{
//    uint8_t byte = 0;
//    uint8_t byte_out = 0;
//    bool worked0;
//    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
//    SoftDemod demod(Mapper::CONST_SET_BPSK);
//
//    std::random_device engine;
//
//
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
//        worked0 = test_mod_soft_demod(mod, demod, ticks, byte, byte_out);
//
//
//        BOOST_CHECK_EQUAL(worked0, true);
//        BOOST_CHECK_EQUAL(byte, byte_out);
//    }
//}
//
//
//
//CSIM_TESX_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_QPSK_SYMBOLS)
//{
////    uint8_t byte = 0;
////    uint8_t byte_out = 0;
////    bool worked0;
//    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_QPSK);
//    SoftDemod demod(Mapper::CONST_SET_QPSK);
//
//    std::random_device engine;
//
//
//
//
//    bool worked2;
//    filter_io_t data, mod_output, demod_output, demod_input;
////    data = byte; // use the =operator overload
////    worked0 = mod.input(data);
////    BOOST_CHECK_EQUAL(worked0, true);
//
//    //demod_input = ComplexDouble(-1,-1);
//
////    vector<ComplexDouble> inputs = {ComplexDouble(-M_SQRT1_2,-M_SQRT1_2), ComplexDouble(M_SQRT1_2,-M_SQRT1_2), ComplexDouble(-M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2/1.20,-M_SQRT1_2/1.20)};
//
//    vector<ComplexDouble> inputs;
//
//    vector<double> b1out;
//    vector<double> fakellr;
//
//    for(size_t i = 0; i < 100; i++)
//    {
//        double v =  -M_SQRT1_2 + (i*M_SQRT1_2/50.0);
//        cout << v << endl;
//        inputs.push_back(ComplexDouble(M_SQRT1_2, v));
//
//
//        double pbi0 = (double)i;
//        double pbi1 = (double)100 - i;
//        fakellr.push_back(log2(pbi0/pbi1));
//    }
//
//    const plotter &plot = plotter::get();
//    plot.nplotqam(inputs, "h");
////    plot.nplot(fakellr, "fake llr");
//
//    for(auto it = inputs.begin(); it != inputs.end(); ++it)
//    {
//        demod_input = *it;
//        demod.input(demod_input);
//        demod.tick();
//        worked2 = demod.output(demod_output);
//        if(worked2) {
//            cout << "input: " << demod_input.rf << endl << " output: " << std::real(demod_output.rf);
//
//
//        }
//
//        demod.tick();
//        worked2 = demod.output(demod_output);
//        if(worked2) {
//            cout << " " << std::real(demod_output.rf) << endl;
//            b1out.push_back(std::real(demod_output.rf));
//
//        }
////        cout << endl << endl << endl << endl;
//    }
//
//    plot.nplot(b1out, "llr of b1");
//
//
////            cout << "worked " << worked2 << endl;
////    if( worked2 )
////    {
////        byte_out = demod_output.byte;
////        cout << "out: " << (int)demod_output.byte << endl;
////    }
//
//
//
//
//
//
////    size_t ticks = sizeof(byte) * CHAR_BIT * MOD_TICKS_PER_SYMBOL;
////
////    for(size_t i = 0; i < 300; i++)
////    {
////
////        if(i < 256)
////        {
////            byte = (uint8_t)i;
////        }
////        else
////        {
////            byte = engine();
////        }
////
////        worked0 = test_mod_demod(mod, demod, ticks, byte, byte_out);
////
////        BOOST_CHECK_EQUAL(worked0, true);
////        BOOST_CHECK_EQUAL(byte, byte_out);
////    }
//}
//
//
//CSIM_TESX_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS)
//{
////    uint8_t byte = 0;
////    uint8_t byte_out = 0;
////    bool worked0;
//    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
//    SoftDemod demod(Mapper::CONST_SET_BPSK);
//
//    std::random_device engine;
//
//
//
//
//    bool worked2;
//    filter_io_t data, mod_output, demod_output, demod_input;
////    data = byte; // use the =operator overload
////    worked0 = mod.input(data);
////    BOOST_CHECK_EQUAL(worked0, true);
//
//    //demod_input = ComplexDouble(-1,-1);
//
////    vector<ComplexDouble> inputs = {ComplexDouble(-M_SQRT1_2,-M_SQRT1_2), ComplexDouble(M_SQRT1_2,-M_SQRT1_2), ComplexDouble(-M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2/1.20,-M_SQRT1_2/1.20)};
//
//    vector<ComplexDouble> inputs;
//
//    vector<double> b1out;
//    vector<double> fakellr;
//
//    double maxswing = 1.0; // this is M_SQRT1_2 for QPSK
//
//    for(size_t i = 0; i < 100; i++)
//    {
//        double v =  -maxswing + (i*maxswing/50.0);
//        cout << v << endl;
//        inputs.push_back(ComplexDouble(v, 0));
//
//
//        double pbi0 = (double)i;
//        double pbi1 = (double)100 - i;
//        fakellr.push_back(log2(pbi0/pbi1));
//    }
//
//    const plotter &plot = plotter::get();
//    plot.nplotqam(inputs, "h");
////    plot.nplot(fakellr, "fake llr");
//
//    for(auto it = inputs.begin(); it != inputs.end(); ++it)
//    {
//        demod_input = *it;
//        demod.input(demod_input);
//        demod.tick();
//        worked2 = demod.output(demod_output);
//        if(worked2) {
//            cout << "input: " << demod_input.rf << endl << " output: " << std::real(demod_output.rf) << endl << endl;
//            b1out.push_back(std::real(demod_output.rf));
//
//        }
//
////        demod.tick();
////        worked2 = demod.output(demod_output);
////        if(worked2) {
////            cout << " " << std::real(demod_output.rf) << endl;
////
////
////        }
////        cout << endl << endl << endl << endl;
//    }
//
//    plot.nplot(b1out, "llr of b0");
//
//
////            cout << "worked " << worked2 << endl;
////    if( worked2 )
////    {
////        byte_out = demod_output.byte;
////        cout << "out: " << (int)demod_output.byte << endl;
////    }
//
//
//
//
//
//
////    size_t ticks = sizeof(byte) * CHAR_BIT * MOD_TICKS_PER_SYMBOL;
////
////    for(size_t i = 0; i < 300; i++)
////    {
////
////        if(i < 256)
////        {
////            byte = (uint8_t)i;
////        }
////        else
////        {
////            byte = engine();
////        }
////
////        worked0 = test_mod_demod(mod, demod, ticks, byte, byte_out);
////
////        BOOST_CHECK_EQUAL(worked0, true);
////        BOOST_CHECK_EQUAL(byte, byte_out);
////    }
//}
//
//
//
//
//
//
//
//CSIM_TESX_CASE(MODULATOR_SOFT_DEMODULATOR_CORRECT_USING_BPSK_SYMBOLS_XXX)
//{
//    Mapper mod(MOD_TICKS_PER_SYMBOL, Mapper::CONST_SET_BPSK);
//    SoftDemod demod(Mapper::CONST_SET_BPSK);
//
//    std::random_device engine;
//
//
//
//
//    bool worked2;
//    filter_io_t data, mod_output, demod_output, demod_input;
////    data = byte; // use the =operator overload
////    worked0 = mod.input(data);
////    BOOST_CHECK_EQUAL(worked0, true);
//
//    //demod_input = ComplexDouble(-1,-1);
//
////    vector<ComplexDouble> inputs = {ComplexDouble(-M_SQRT1_2,-M_SQRT1_2), ComplexDouble(M_SQRT1_2,-M_SQRT1_2), ComplexDouble(-M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2, M_SQRT1_2), ComplexDouble(M_SQRT1_2/1.20,-M_SQRT1_2/1.20)};
//
//    vector<ComplexDouble> inputs;
//
//    vector<double> b1out;
//    vector<double> fakellr;
//
//    double maxswing = 1.0; // this is M_SQRT1_2 for QPSK
//
//    for(size_t i = 0; i < 100; i++)
//    {
//        double v =  -maxswing + (i*maxswing/50.0);
//        cout << v << endl;
//        inputs.push_back(ComplexDouble((i%2)*1.9-2, 0));
//
//
//        double pbi0 = (double)i;
//        double pbi1 = (double)100 - i;
//        fakellr.push_back(log2(pbi0/pbi1));
//    }
//
//    const plotter &plot = plotter::get();
//    plot.nplotqam(inputs, "h");
////    plot.nplot(fakellr, "fake llr");
//
//    for(auto it = inputs.begin(); it != inputs.end(); ++it)
//    {
//        demod_input = *it;
//        demod.input(demod_input);
//        demod.tick();
//        worked2 = demod.output(demod_output);
//        if(worked2) {
//            cout << "input: " << demod_input.rf << endl << " output: " << std::real(demod_output.rf) << endl << endl;
//            b1out.push_back(std::real(demod_output.rf));
//
//        }
//
////        demod.tick();
////        worked2 = demod.output(demod_output);
////        if(worked2) {
////            cout << " " << std::real(demod_output.rf) << endl;
////
////
////        }
////        cout << endl << endl << endl << endl;
//    }
//
//    plot.nplot(b1out, "llr of b0");
//
//
//}


CSIM_TEST_SUITE_END()
