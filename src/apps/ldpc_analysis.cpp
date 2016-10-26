#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <filters/mapper.hpp>
#include <filters/sine_wave.hpp>
#include <filters/automatic_gain.hpp>
#include <filters/mixer.hpp>
#include <filters/decimator.hpp>
#include <filters/hard_demod.hpp>
#include <filters/noise_element.hpp>

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

static void constructRadiosForLDPC(RadioSet &rs, double distance, double ebn0)
{
    std::vector<std::pair<double, double> > positions(2);
    //Positions are irrelevant, noise is generated in rx chain per ebn0 param
    positions[0] = std::pair<double, double>(0.0, 0.0);
    positions[1] = std::pair<double, double>(0.0, 0.0);
    construct_radio_set_ldpc_ebn0(rs, positions, 30);
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
                    std::cout << "radio #" << id << " received " << (int)rxByte << " bitDiff = " << bitDiff << std::endl;
                }
            });

    while(true) {
        if (rxCount >= numIterations) {
            std::cout << "Transmitted and received " << rxCount << " bytes. Exiting..." << std::endl;
            break;
        }
        uint8_t byte1 = std::rand() % 256;
        uint8_t byte2 = std::rand() % 256;
        if (didReceive) {
            world.sendByte(SENDING_RADIO_ID, byte1);
            world.sendByte(SENDING_RADIO_ID, byte2);
            lastSent = byte2; //FIXME
            didReceive = false;
        }

        world.tick();
    }
    return bitDiff;
}

int main(int argc, char *argv[])
{
    static constexpr size_t NUM_BYTES_TX_RX_DESIRED = 2000;
    static constexpr double EBN_LOW = -3;
    static constexpr double EBN_HIGH = 10;
    log_info("Starting Bit Error Rate tester!");
    std::srand(1473294057+1);

    SigWorld world;
    RadioSet rs;

    const plotter &plot = plotter::get();


    std::vector<std::vector<double> > bers;
    std::vector<std::vector<double> > ebn0s;
    std::vector<std::string> titles;

    std::vector<double> ber1;
    std::vector<double> ebn1;

    //For each modulation scheme, sweep through distances from 0 - 15km
    for (double ebn0 = EBN_LOW; ebn0 <= EBN_HIGH; ebn0 += 1) {
        constructRadiosForLDPC(rs, 0, ebn0); //construct 2 radios, 'distance' meters apart
        world.init(&rs);
        unsigned int bitDiff = runTrial(world, NUM_BYTES_TX_RX_DESIRED); //send / receive NUM_BYTES_TX_RX_DESIRED bytes
        world.reset();
        double ber = (double)bitDiff / (double)(NUM_BYTES_TX_RX_DESIRED * 8);// << ", " << NUM_BYTES_TX_RX_DESIRED * 8;
        //Record results, CSV
//        std::cout << modSchemeToString(scheme) << ", " << ebn0 << ", " << ber << std::endl;
        ber1.push_back(ber);
        ebn1.push_back(ebn0);
    }

    bers.push_back(ber1);
    ebn0s.push_back(ebn1);
    titles.push_back("BPSK");

    std::vector<double> theoryber;
    std::vector<double> theoryebn;


    for (double ebn0 = EBN_LOW; ebn0 <= EBN_HIGH; ebn0 += 1) {

        double theory = 0.5 * erfc(sqrt(pow(10, ebn0/10)));
        theoryber.push_back(theory);
        theoryebn.push_back(ebn0);
    }

    bers.push_back(theoryber);
    ebn0s.push_back(theoryebn);
    titles.push_back("BPSK Theory");



    plot.nplotber(bers, ebn0s, titles, "first");

    usleep(1000000.0); // move this into plotter somehow

    return 0;
}
