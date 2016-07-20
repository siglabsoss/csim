
#include "radio_s.hpp"
#include "dummy_filter_element.hpp"
#include "fixedfir.h"

int main(int argc, char *argv[])
{
    //Construct a radio with some dummy filter chains
    FixedComplex<16> tap[10]; //XXX temporarily here

    FilterChain *mod = new FilterChain(new DummyByte(new DummyByte(new DummyByte(new DummyByteToComplex(new DummyComplex(nullptr))))));
    FilterChain *demod = new FilterChain(new fixedfir(10, tap, new DummyComplex(new DummyComplex(new DummyComplex(new DummyComplexToByte(new DummyByte(new DummyByte(nullptr))))))));
    RadioS radio(Vector2d(0,0), *mod, *demod);

    uint8_t byte;

    //Receive an RF sample, which in turn feeds through the demodulation filter chain
    std::complex<double> sample(1.123456, 2.0);
    radio.rxWave(sample);
    bool didOutput = radio.rxByte(byte);
    if (didOutput) {
        std::cout << "Went from " << sample << " to " << static_cast<int>(byte) << std::endl;
    } else {
        std::cout << "Fed sample into demodulator but did not get an output" << std::endl;
    }

    //Transmit a byte, which in turn feeds through the modulation filter chain
    byte = 0xAA;
    radio.txByte(byte);
    didOutput = radio.txWave(sample);
    if (didOutput) {
        std::cout << "Went from " << static_cast<int>(byte) << " to " << sample << std::endl;
    } else {
        std::cout << "Fed sample into modulator but did not get an output" << std::endl;
    }
    return 0;
}
