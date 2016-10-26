#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <filters/mapper.hpp>
#include <filters/sine_wave.hpp>
#include <filters/automatic_gain.hpp>
#include <filters/mixer.hpp>
#include <filters/decimator.hpp>
#include <filters/hard_demapper.hpp>

#include <utils/utils.hpp>
#include <utils/radio_utils.hpp>

#include <probes/sample_count_trigger.hpp>
#include <probes/tick_count_trigger.hpp>
#include <probes/level_trigger.hpp>

#include <sys/time.h>
#include <utility>

#include "retrosim_params.hpp"

static constexpr radio_id_t SENDING_RADIO_ID = 0;
static constexpr radio_id_t RECEIVING_RADIO_ID = 1;

static void constructRadiosForBER(RadioSet &rs, double distance, Mapper::constellation_set_t scheme)
{
    std::vector<std::pair<double, double> > positions(2);
    positions[0] = std::pair<double, double>(0, 0);
    positions[1] = std::pair<double, double>(0, distance);
    construct_radio_set(rs, positions, scheme);
}

static unsigned int runTrial(SigWorld &world, size_t numIterations)
{
    bool didReceive = true;
    size_t rxCount = 0;
    uint8_t lastSent = 0;
    unsigned int bitDiff = 0;
    world.didReceiveByte([&didReceive, &rxCount, &lastSent, &bitDiff](radio_id_t id, uint8_t rxByte)
            {
                if (id == RECEIVING_RADIO_ID) {
                    didReceive = true;
                    rxCount++;
                    bitDiff += utils::calculateHammingDistance(lastSent, rxByte);
                    //std::cout << "radio #" << id << " received " << (int)rxByte << " bitDiff = " << bitDiff << std::endl;
                }
            });

    while(true) {
        if (rxCount >= numIterations) {
            std::cout << "Transmitted and received " << rxCount << " bytes. Exiting..." << std::endl;
            break;
        }
        int random_variable = std::rand();
        uint8_t byte = random_variable % 256;
        if (didReceive && world.sendByte(SENDING_RADIO_ID, byte)) {
            lastSent = byte;
            //std::cout << "radio #" << SENDING_RADIO_ID << " sent     " << (int)byte << std::endl;
            didReceive = false;
        }

        world.tick();
    }
    return bitDiff;
}

static std::string modSchemeToString(Mapper::constellation_set_t scheme)
{
    switch(scheme) {
        default:
        case Mapper::CONST_SET_NULL:
            return "*(NULL)";
        case Mapper::CONST_SET_BPSK:
            return "*BPSK";
        case Mapper::CONST_SET_QPSK:
            return "*QPSK";
        case Mapper::CONST_SET_8PSK:
            return "*8PSK";
        case Mapper::CONST_SET_QAM16:
            return "*QAM16";
    }
}

static Mapper::constellation_set_t getNextScheme(Mapper::constellation_set_t scheme)
{
    switch(scheme) {
        default:
        case Mapper::CONST_SET_NULL:
            return Mapper::CONST_SET_NULL;
        case Mapper::CONST_SET_BPSK:
            return Mapper::CONST_SET_QPSK;
        case Mapper::CONST_SET_QPSK:
            return Mapper::CONST_SET_QAM16; //skipping 8PSK
        case Mapper::CONST_SET_8PSK:
            return Mapper::CONST_SET_QAM16;
        case Mapper::CONST_SET_QAM16:
            return Mapper::CONST_SET_NULL;
    }
}

int main(int argc, char *argv[])
{
    static constexpr size_t NUM_BYTES_TX_RX_DESIRED = 1000;
    log_info("Starting Bit Error Rate tester!");
    std::srand(std::time(0));

    SigWorld world;
    RadioSet rs;

    //Sweep through modulation schemes
    for (Mapper::constellation_set_t scheme = Mapper::CONST_SET_BPSK; scheme != Mapper::CONST_SET_NULL; scheme = getNextScheme(scheme)) {
        //For each modulation scheme, sweep through distances from 0 - 15km
        for (double distance = 0; distance <= 15000; distance += 100) {
            constructRadiosForBER(rs, distance, scheme); //construct 2 radios, 'distance' meters apart
            world.init(&rs); // include noise, no delay, and no phase rotation
            unsigned int bitDiff = runTrial(world, NUM_BYTES_TX_RX_DESIRED); //send / receive NUM_BYTES_TX_RX_DESIRED bytes
            world.reset();
            //Record results, CSV
            std::cout << modSchemeToString(scheme) << ", " << distance << ", " << bitDiff << ", " << (double)bitDiff / (double)(NUM_BYTES_TX_RX_DESIRED * 8) << ", " << NUM_BYTES_TX_RX_DESIRED * 8 << std::endl;
        }
    }
    return 0;
}
