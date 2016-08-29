#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <filters/complex_to_fixed.hpp>
#include <filters/fixed_to_byte.hpp>
#include <filters/modulator.hpp>
#include <filters/sine_wave.hpp>
#include <filters/automatic_gain.hpp>
#include <filters/fixedfft.hpp>
#include <filters/mixer.hpp>
#include <filters/zero_pad_interpolator.hpp>
#include <filters/linear_gain_amplifier.hpp>
#include <probes/sample_count_trigger.hpp>
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

void constructRadios(SigWorld &world)
{
    for (int i = 0; i < 1; i++) {
        world.addRadio([i]()
                {
                    radio_config_t config {
                        .position = Vector2d(0.0, i*1199.6*10),
                        .id = static_cast<radio_id_t>(i + 1)
                    };

                    constexpr size_t FFT_WINDOW_SIZE = 1024;

                    FilterChain modulation_chain;
                    Modulator           *qam16    = new Modulator(Modulator::MOD_SCHEME_QAM16);
                    fixedfft            *fft      = new fixedfft(FFT_WINDOW_SIZE, 0);
                    ZeroPadInterpolator *zpi      = new ZeroPadInterpolator(FFT_WINDOW_SIZE);
                    LinearGainAmplifier *lga      = new LinearGainAmplifier(2);
                    fixedfft           *ifft     = new fixedfft(FFT_WINDOW_SIZE*2, 0, true);
                    //SampleCountTrigger  *tp1      = new SampleCountTrigger("FFT trigger", 1000, 1, 10000);
                    LevelTrigger        *lt1      = new LevelTrigger("FFT Level", 1000, 10);
                    SampleCountTrigger  *tp2      = new SampleCountTrigger("IFFT trigger", 1000, 1, 10000);
                    SampleCountTrigger  *tp3      = new SampleCountTrigger("Mixer trigger", 1000, 1, 8000);
                    SampleCountTrigger  *tp4      = new SampleCountTrigger("QAM trigger", 1000, 1, 15000);

                    //XXX use parameter for frequency
                    Mixer *mixer                = new Mixer(3);

                    /*
                    qam16->shouldPublish(true);
                    fft->shouldPublish(true);
                    zpi->shouldPublish(true);
                    lga->shouldPublish(true);
                    ifft->shouldPublish(true);
                    mixer->shouldPublish(true);
                    */

                    modulation_chain = *tp3 + *mixer + *tp2 +  *ifft + *lga + *zpi + *lt1 +  *fft + *tp4 + *qam16;

                    FilterChain demodulation_chain;
                    AutomaticGain *ag = new AutomaticGain();
                    //ag->shouldPublish(true);
                    demodulation_chain = *ag;

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                });
    }
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
