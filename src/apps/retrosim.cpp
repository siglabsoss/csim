#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <utils/radio_utils.hpp>

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


int main(int argc, char *argv[])
{

    log_info("Starting RetroSim!");

    SigWorld world;
    RadioSet rs;
    std::vector<std::pair<double, double> > positions(2);
    positions[0] = std::pair<double, double>(0, 0);
    positions[1] = std::pair<double, double>(0, 119.96);
    construct_radio_set(rs, positions);
    world.init(&rs);
    bool didReceive = true;
    world.didReceiveByte([&didReceive](radio_id_t id, uint8_t byte)
            {
                std::cout << "radio #" << id << " received " << (int)byte << std::endl;
                didReceive = true;
            });

    size_t tickCount = 0;
    struct timespec tp1;
    struct timespec tp2;
    std::srand(std::time(0));
    while(true) {
        int random_variable = std::rand();
        uint8_t byte = random_variable % 256;
        if (didReceive && world.sendByte(0, byte)) {
            std::cout << "radio #" << 0 << " sent     " << (int)byte << std::endl;
            didReceive = false;
        }
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
