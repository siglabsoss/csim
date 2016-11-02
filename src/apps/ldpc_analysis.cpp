#include <core/sigworld.hpp>
#include <core/logger.hpp>

#include <utils/utils.hpp>
#include <utils/radio_utils.hpp>

#include <utils/plotter.hpp>

static constexpr radio_id_t SENDING_RADIO_ID = 0;
static constexpr radio_id_t RECEIVING_RADIO_ID = 1;
static constexpr size_t     NUM_BITS_PER_TRANSMISSION = 324;
static constexpr size_t     NUM_TRANSMISSION_ITERATIONS = 30;
static constexpr size_t     NUM_BIT_ERRORS_PER_EBN0 = 100;

static void constructRadiosForLDPC(RadioSet &rs, double distance, double ebn0)
{
    std::vector<std::pair<double, double> > positions(2);
    //Positions are irrelevant, noise is generated in rx chain per ebn0 param
    positions[0] = std::pair<double, double>(0.0, 0.0);
    positions[1] = std::pair<double, double>(0.0, 0.0);
    construct_radio_set_ldpc_ebn0(rs, positions, ebn0);
}

static void runTrial(SigWorld &world, size_t numIterations, unsigned int &bitErrors, unsigned int &numTxBits)
{
    bool didFinishIteration = true;
    size_t rxIterations = 0;
    size_t rxCount = 0;
    numTxBits = 0;
    bitErrors = 0;
    std::vector<bool> txBits(NUM_BITS_PER_TRANSMISSION);
    std::vector<bool> rxBits(NUM_BITS_PER_TRANSMISSION);
    world.didReceiveByte([&didFinishIteration, &rxIterations, &rxCount, &rxBits, &txBits, &bitErrors, &numTxBits](radio_id_t id, bool rxBit)
            {
                if (id == RECEIVING_RADIO_ID) {
                    rxBits[NUM_BITS_PER_TRANSMISSION - 1 - rxCount] = rxBit;
                    ++rxCount;
                    if (rxCount == NUM_BITS_PER_TRANSMISSION) {
                        didFinishIteration = true;
                        ++rxIterations;
                        rxCount = 0;
                        size_t additionalBitErrors = 0;
                        for (size_t i = 0; i < NUM_BITS_PER_TRANSMISSION; i++) {
                            if (txBits[i] != rxBits[i]) {
                                additionalBitErrors++;
                            }

                        }
                        bitErrors += additionalBitErrors;
                        if (additionalBitErrors > 0) {
                            std::cout << "tx:";
                            for (size_t i = 0; i < NUM_BITS_PER_TRANSMISSION; ++i) {
                                if ((i % 8) == 0) {
                                    std::cout << " ";
                                }
                                std::cout << txBits[i] << ",";
                            }
                            std::cout << std::endl;
                            std::cout << "rx:";
                            for (size_t i = 0; i < NUM_BITS_PER_TRANSMISSION; i++) {
                                if (i % 8 == 0) {
                                    std::cout << " ";
                                }
                                std::cout << rxBits[i] << ",";
                            }
                            std::cout << std::endl;
                            std::cout << "NUMBER OF BIT ERRORS = " << bitErrors << " (+" << additionalBitErrors << ")" << std::endl;
                        }
                    }
//                    bitErrors += (rxBit == expectedBit) ? 0 : 1;
                    ++numTxBits;
//                    std::cout << "radio #" << id << " received " << (int)rxBit << " bitDiff = " << bitDiff << std::endl;
                }
            });

    while(true) {
        if (rxIterations >= numIterations || bitErrors >= NUM_BIT_ERRORS_PER_EBN0) {
            std::cout << "Transmitted and received " << numTxBits / 8 << " bytes." << std::endl;
            std::cout << "Total bit errors = " << bitErrors << std::endl;
            break;
        }

        if (didFinishIteration) {
            for (size_t i = 0; i < NUM_BITS_PER_TRANSMISSION; ++i) {
                bool bit = static_cast<bool>(std::rand() & 1);
                txBits[txBits.size() - 1 - i] = bit;
            }

            world.sendBits(SENDING_RADIO_ID, txBits);
            didFinishIteration = false;
        }

        world.tick();
    }
}

int main(int argc, char *argv[])
{
    static constexpr double EBN_LOW = -3.0;
    static constexpr double EBN_HIGH = 20.0;
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

    for (double ebn0 = EBN_LOW; ebn0 <= EBN_HIGH; ebn0 += 1.0) {
        constructRadiosForLDPC(rs, 0, ebn0); //construct 2 radios, 'distance' meters apart
        world.init(&rs);
        unsigned int bitErrors, totalBits;
        runTrial(world, NUM_TRANSMISSION_ITERATIONS, bitErrors, totalBits); //send / receive NUM_BYTES_TX_RX_DESIRED bytes
        world.reset();
        double ber = (double)bitErrors / (double)totalBits;

        ber1.push_back(ber);
        std::cout << "BER @ Eb/N0 of " << ebn0 << " = " << ber << std::endl;
        ebn1.push_back(ebn0);
    }

    bers.push_back(ber1);
    ebn0s.push_back(ebn1);
    titles.push_back("BPSK");

    std::vector<double> theoryber;
    std::vector<double> theoryebn;


    for (double ebn0 = EBN_LOW; ebn0 <= EBN_HIGH; ebn0 += 1.0) {
        double theory = 0.5 * erfc(sqrt(pow(10, ebn0/10.0)));
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
