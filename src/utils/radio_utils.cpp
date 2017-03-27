#include <utils/radio_utils.hpp>
#include <utils/ldpc_utils.hpp>

#include <filters/demapper.hpp>
#include <filters/noise_element.hpp>
#include <filters/ldpc_encode.hpp>
#include <filters/ldpc_decoder.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <filters/fft.hpp>
#include <utils/utils.hpp>

static void construct_ldpc_ebn0_tx(FilterChain &txChain)
{
    std::vector<std::vector<bool> > G = LDPCUtils::getBitArrayFromCSV("data/ldpc/80211n_G_648_12.csv");

    LDPCEncode * encode  = new LDPCEncode(G);
    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, 1024);
    Mapper *     mapper   = new Mapper(mcs);

    txChain = *mapper + *encode;
}

static void construct_ldpc_enb0_rx(FilterChain &rxChain, double ebn0)
{
    std::vector<std::vector<bool> > H = LDPCUtils::getBitArrayFromCSV("data/ldpc/80211n_H_648_12.csv");

    LDPCDecoder * decode = new LDPCDecoder(H);
    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, 1024);
    Demapper * demapper = new Demapper(mcs, false);

    double noiseVar = 1.0 / pow(10, ebn0/10.0);
    NoiseElement * ne    = new NoiseElement(noiseVar);

    rxChain = *decode + *demapper + *ne;
}

void construct_radio_set_ldpc_ebn0(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, double ebn0)
{
    size_t count = 0;
    for (auto it = coords.begin(); it != coords.end(); it++) {
        rs.addRadio([it, count, ebn0]()
                {
                    radio_config_t config {
                        .position = Vector2d(it->first, it->second),
                        .id = static_cast<radio_id_t>(count)
                    };
                    std::cout << "Adding radio with id = " << count << std::endl;

                    FilterChain modulation_chain;
                    construct_ldpc_ebn0_tx(modulation_chain);

                    FilterChain demodulation_chain;
                    construct_ldpc_enb0_rx(demodulation_chain, ebn0);

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                });
        count++;
    }
    rs.init(true, true, true); //no noise, no delay, no phase rotation
}

