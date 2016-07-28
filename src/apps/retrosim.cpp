#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <filters/complex_to_fixed.hpp>
#include <filters/fixed_to_byte.hpp>
#include <filters/fixedfir.hpp>
#include <filters/sine_wave.hpp>

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

int main(int argc, char *argv[])
{

    log_info("Starting RetroSim!");

    SigWorld world;

    radio_config_t config;

    for (int i = 0; i < 5; i++) {
        config.position = Vector2d(0.0, i*1199.6);
        config.id = i+1;
        world.addRadio([](const radio_config_t &config)
                {
                    FilterChain modulation_chain;

                    SineWave *sw = new SineWave(2000);
                    //modulation_chain = *dc4 + *dbtc + *db6 + *db5 + *db4;
                    modulation_chain = *sw;

                    FilterChain demodulation_chain;
                    sw = new SineWave(300);
                    demodulation_chain = *sw;

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                }, config);
    }

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
