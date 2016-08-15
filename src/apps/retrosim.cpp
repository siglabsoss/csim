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
#include <probes/sample_count_trigger.hpp>
#include <probes/tick_count_trigger.hpp>
#include <probes/level_trigger.hpp>

#include <sys/time.h>
#include <utility>

#include "retrosim_params.hpp"

timespec timediff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

static constexpr size_t FFT_WINDOW_SIZE = 1024;
static constexpr size_t UPSAMPLE_FACTOR = 2;
static constexpr size_t MOD_TICKS_PER_SYMBOL = 2;
static constexpr size_t MIXER_TICKS_PER_PERIOD = (MOD_TICKS_PER_SYMBOL / UPSAMPLE_FACTOR) * 10;

void constructModulationChain(FilterChain &modChain, bool withTriggers)
{
    Modulator           *qam16    = new Modulator(MOD_TICKS_PER_SYMBOL, Modulator::MOD_SCHEME_QAM16);
    fixedfft            *fft      = new fixedfft(FFT_WINDOW_SIZE, 0);
    ZeroPadInterpolator *zpi      = new ZeroPadInterpolator(FFT_WINDOW_SIZE, UPSAMPLE_FACTOR);
    LinearGainAmplifier *lga      = new LinearGainAmplifier(UPSAMPLE_FACTOR);
    fixedifft           *ifft     = new fixedifft(FFT_WINDOW_SIZE*UPSAMPLE_FACTOR, 0);
    Mixer *upmixer              = new Mixer(MIXER_TICKS_PER_PERIOD, true /* upmix */);

    //modulation_chain = *tp3 + *upmixer + *tp2 +  *ifft + *lga + *zpi + *lt1 +  *fft + *tp4 + *qam16;
    if (withTriggers) {
        TickCountTrigger  *lt1      = new TickCountTrigger("FFT trigger", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*4);
        TickCountTrigger  *tp2      = new TickCountTrigger("IFFT trigger", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*4);
        TickCountTrigger  *tp3      = new TickCountTrigger("Mixer trigger", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*4);
        TickCountTrigger  *tp4      = new TickCountTrigger("QAM trigger", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*4);
        TickCountTrigger  *tp5      = new TickCountTrigger("ZPI trigger", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*4);
        TickCountTrigger  *tp6      = new TickCountTrigger("LGA trigger", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*4);
        modChain = *tp3 + *upmixer + *tp2 + *ifft + *tp6 + *lga + *tp5 + *zpi + *lt1 + *fft + *tp4 + *qam16;
    } else {
        modChain = *upmixer + *ifft + *lga + *zpi + *fft + *qam16;
    }
}

void constructDemodulationChain(FilterChain &demodChain, bool withTriggers)
{
    Mixer *downmixer = new Mixer(10, false /* downmix */);
    TickCountTrigger *lt2  = new TickCountTrigger("Downmix", FFT_WINDOW_SIZE*10, 1, FFT_WINDOW_SIZE*5);
    downmixer->shouldPublish(true);

    if (withTriggers) {
        demodChain = *lt2 + *downmixer;
    } else {
        demodChain = *downmixer;
    }
}

void constructRadios(SigWorld &world)
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
    world.addRadio([]()
            {
                radio_config_t config {
                    .position = Vector2d(0.0, 10*11.996),
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

    constructRadios(world);

    world.init();

    size_t tickCount = 0;
    struct timespec tp1;
    struct timespec tp2;

    while(true) {
        if (tickCount == 0) {
            clock_gettime(CLOCK_REALTIME, &tp1);
        }

        world.tick();

        if (tickCount < 1000000) {
            tickCount++;
        } else {
            tickCount = 0;
            clock_gettime(CLOCK_REALTIME, &tp2);
            timespec diff = timediff(tp1, tp2);
            double diff_seconds = diff.tv_sec + (diff.tv_nsec / 10.0e9);
            log_debug("Running at %fHz", 1000000 / diff_seconds);
        }
    }
    return 0;
}
