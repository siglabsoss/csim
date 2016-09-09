#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <filters/modulator.hpp>
#include <filters/sine_wave.hpp>
#include <filters/automatic_gain.hpp>
#include <filters/fixedfft.hpp>
#include <filters/mixer.hpp>
#include <filters/zero_pad_interpolator.hpp>
#include <filters/linear_gain_amplifier.hpp>
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

static void constructRadiosForEbn0(RadioSet &rs, double distance, Modulator::mod_scheme_t scheme, double ebn0)
{
    std::vector<std::pair<double, double> > positions(2);
    positions[0] = std::pair<double, double>(0, 0);
    positions[1] = std::pair<double, double>(0, distance);
    construct_radio_set_ebn0(rs, positions, ebn0, scheme);
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
                    bitDiff += calculateHammingDistance(lastSent, rxByte);
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

_UNUSED_ static std::string modSchemeToString(Modulator::mod_scheme_t scheme)
{
    switch(scheme) {
        default:
        case Modulator::MOD_SCHEME_NULL:
            return "*(NULL)";
        case Modulator::MOD_SCHEME_BPSK:
            return "*BPSK";
        case Modulator::MOD_SCHEME_QPSK:
            return "*QPSK";
        case Modulator::MOD_SCHEME_8PSK:
            return "*8PSK";
        case Modulator::MOD_SCHEME_QAM16:
            return "*QAM16";
    }
}

_UNUSED_ static Modulator::mod_scheme_t getNextScheme(Modulator::mod_scheme_t scheme)
{
    switch(scheme) {
        default:
        case Modulator::MOD_SCHEME_NULL:
            return Modulator::MOD_SCHEME_NULL;
        case Modulator::MOD_SCHEME_BPSK:
            return Modulator::MOD_SCHEME_QPSK;
        case Modulator::MOD_SCHEME_QPSK:
            return Modulator::MOD_SCHEME_QAM16; //skipping 8PSK
        case Modulator::MOD_SCHEME_8PSK:
            return Modulator::MOD_SCHEME_QAM16;
        case Modulator::MOD_SCHEME_QAM16:
            return Modulator::MOD_SCHEME_NULL;
    }
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


    vector<vector<double> > bers;
    vector<vector<double> > ebn0s;
    vector<string> titles;

    vector<double> ber1;
    vector<double> ebn1;



    //Sweep through modulation schemes
    Modulator::mod_scheme_t scheme = Modulator::MOD_SCHEME_BPSK;

    //For each modulation scheme, sweep through distances from 0 - 15km
    for (double ebn0 = EBN_LOW; ebn0 <= EBN_HIGH; ebn0 += 1) {
        constructRadiosForEbn0(rs, 0, scheme, ebn0); //construct 2 radios, 'distance' meters apart
        world.init(&rs); // include noise, no delay, and no phase rotation
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

    vector<double> theoryber;
    vector<double> theoryebn;


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
