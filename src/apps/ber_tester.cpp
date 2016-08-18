#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <filters/complex_to_fixed.hpp>
#include <filters/fixed_to_byte.hpp>
#include <filters/modulator.hpp>
#include <filters/sine_wave.hpp>
#include <filters/automatic_gain.hpp>
#include <filters/fixedfft.hpp>
#include <filters/fixedifft.hpp>
#include <filters/mixer.hpp>
#include <filters/zero_pad_interpolator.hpp>
#include <filters/linear_gain_amplifier.hpp>
#include <filters/decimator.hpp>
#include <filters/hard_demod.hpp>
#include <utils/utils.hpp>

#include <probes/sample_count_trigger.hpp>
#include <probes/tick_count_trigger.hpp>
#include <probes/level_trigger.hpp>

#include <sys/time.h>
#include <utility>

#include "retrosim_params.hpp"

static constexpr size_t FFT_WINDOW_SIZE = 1024;
static constexpr size_t UPSAMPLE_FACTOR = 1;
static constexpr size_t MOD_TICKS_PER_SYMBOL = 4;
//ticks per symbol needs to be greater than upsample factor in order to not saturate
static constexpr size_t MIXER_TICKS_PER_PERIOD = (MOD_TICKS_PER_SYMBOL / UPSAMPLE_FACTOR) * 1;
static constexpr size_t NUM_BYTES_TX_RX_DESIRED = 1000;

void constructModulationChain(FilterChain &modChain, bool withTriggers)
{
    Modulator           *qam16    = new Modulator(MOD_TICKS_PER_SYMBOL, Modulator::MOD_SCHEME_QAM16);
    fixedfft            *fft      = new fixedfft(FFT_WINDOW_SIZE, 0);
    ZeroPadInterpolator *zpi      = new ZeroPadInterpolator(FFT_WINDOW_SIZE, UPSAMPLE_FACTOR);
    LinearGainAmplifier *lga      = new LinearGainAmplifier(UPSAMPLE_FACTOR);
    fixedifft           *ifft     = new fixedifft(FFT_WINDOW_SIZE*UPSAMPLE_FACTOR, 0);
    Mixer *upmixer                = new Mixer(MIXER_TICKS_PER_PERIOD, true /* upmix */);

    if (withTriggers) {
        SampleCountTrigger  *tp4      = new SampleCountTrigger("QAM trigger",   FFT_WINDOW_SIZE*1, 1,                       FFT_WINDOW_SIZE*0);
        SampleCountTrigger  *lt1      = new SampleCountTrigger("FFT trigger",   FFT_WINDOW_SIZE*1, 1,                       FFT_WINDOW_SIZE*0);
        SampleCountTrigger  *tp5      = new SampleCountTrigger("ZPI trigger",   FFT_WINDOW_SIZE*UPSAMPLE_FACTOR, 1,         FFT_WINDOW_SIZE*0);
        SampleCountTrigger  *tp6      = new SampleCountTrigger("LGA trigger",   FFT_WINDOW_SIZE*UPSAMPLE_FACTOR, 1,         FFT_WINDOW_SIZE*0);
        SampleCountTrigger  *tp2      = new SampleCountTrigger("IFFT trigger",  FFT_WINDOW_SIZE*UPSAMPLE_FACTOR, 1,         FFT_WINDOW_SIZE*0);
        SampleCountTrigger  *tp3      = new SampleCountTrigger("Mixer trigger", FFT_WINDOW_SIZE*MIXER_TICKS_PER_PERIOD, 1,  FFT_WINDOW_SIZE*0);

        //modChain = *tp3 + *upmixer + *tp2 + *ifft + *tp6 + *lga + *tp5 + *zpi + *lt1 + *fft + *tp4 + *qam16;
        modChain = *tp3 + *upmixer + *tp4 + *qam16;
    } else {
        //modChain = *upmixer + *ifft + *lga + *zpi + *fft + *qam16;
        modChain = *upmixer + *qam16;
    }
}

void constructDemodulationChain(FilterChain &demodChain, bool withTriggers)
{
    Mixer *downmixer = new Mixer(MIXER_TICKS_PER_PERIOD, false /* downmix */);
    Decimator *decim = new Decimator(MOD_TICKS_PER_SYMBOL, 0);
    HardDemod *demod = new HardDemod(Modulator::MOD_SCHEME_QAM16, 0.0);

    if (withTriggers) {
        SampleCountTrigger *lt2  = new SampleCountTrigger("Downmix",    FFT_WINDOW_SIZE*MIXER_TICKS_PER_PERIOD, 1,     FFT_WINDOW_SIZE*0);
        SampleCountTrigger *lt3  = new SampleCountTrigger("Decimation", FFT_WINDOW_SIZE*1, 1,                          FFT_WINDOW_SIZE*0);
        //demodChain = *lt3 + *decim + *lt2 + *downmixer;
        demodChain = *demod + *lt3 + *decim + *lt2 + *downmixer;
    } else {
        demodChain = *demod + *decim + *downmixer;
    }
}

void constructRadios(SigWorld &world, double distance)
{
    world.addRadio([]()
            {
                radio_config_t config {
                    .position = Vector2d(0.0, 0.0),
                    .id = static_cast<radio_id_t>(0)
                };

                FilterChain modulation_chain;
                constructModulationChain(modulation_chain, false);

                FilterChain demodulation_chain;
                constructDemodulationChain(demodulation_chain, false);

                return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
            });
    world.addRadio([distance]()
            {
                radio_config_t config {
                    .position = Vector2d(0.0, distance),
                    .id = static_cast<radio_id_t>(1)
                };

                FilterChain modulation_chain;
                constructModulationChain(modulation_chain, false);

                FilterChain demodulation_chain;
                constructDemodulationChain(demodulation_chain, false);

                return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
            });
}

int main(int argc, char *argv[])
{

    log_info("Starting RetroSim!");

    SigWorld world;
    constructRadios(world, 7*119.96);
    world.init();
    bool didReceive = true;
    size_t rxCount = 0;
    uint8_t lastSent = 0;
    unsigned int bitDiff = 0;
    world.didReceiveByte([&didReceive, &rxCount, &lastSent, &bitDiff](radio_id_t id, uint8_t rxByte)
            {
                didReceive = true;
                rxCount++;
                bitDiff += calculateHammingDistance(lastSent, rxByte);
                std::cout << "radio #" << id << " received " << (int)rxByte << " bitDiff = " << bitDiff << std::endl;
            });

    std::srand(std::time(0));
    while(true) {
        if (rxCount >= NUM_BYTES_TX_RX_DESIRED) {
            std::cout << "Transmitted and received " << rxCount << " bytes. Exiting..." << std::endl;
            break;
        }
        int random_variable = std::rand();
        uint8_t byte = random_variable % 256;
        if (didReceive && world.sendByte(0, byte)) {
            lastSent = byte;
            std::cout << "radio #" << 0 << " sent     " << (int)byte << std::endl;
            didReceive = false;
        }

        world.tick();
    }

    std::cout << "Total number of bit errors = " << bitDiff << " total number of bits sent = " << NUM_BYTES_TX_RX_DESIRED * 8 << " BER = " << (double)bitDiff / (double)(NUM_BYTES_TX_RX_DESIRED * 8) << std::endl;
    return 0;
}
