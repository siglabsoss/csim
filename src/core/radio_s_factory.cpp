#include <core/radio_s.hpp>

#include <filters/dummy_filter_element.hpp>
#include <filters/complex_to_fixed.hpp>
#include <filters/fixed_to_byte.hpp>
#include <filters/fixedfir.hpp>
#include <filters/sine_wave.hpp>

RadioS * RadioS::create(radio_config_t &config)
{
    FilterChain modulation_chain;

    SineWave *sw = new SineWave(1000);
    //modulation_chain = *dc4 + *dbtc + *db6 + *db5 + *db4;
    modulation_chain = *sw;

    FilterChain demodulation_chain;
    ComplexToFixed *ctf = new ComplexToFixed;
    FixedComplex<16> taps[10];
    fixedfir *fir = new fixedfir(10, taps);
    FixedToByte *ftb = new FixedToByte;

    demodulation_chain = *ftb + *fir + *ctf;

    return new RadioS(config, modulation_chain, demodulation_chain);
}
