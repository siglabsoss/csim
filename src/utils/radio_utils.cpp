#include <utils/radio_utils.hpp>
#include <utils/ldpc_utils.hpp>

#include <filters/mixer.hpp>
#include <filters/decimator.hpp>
#include <filters/demapper.hpp>
#include <filters/noise_element.hpp>
#include <filters/ldpc_encode.hpp>
#include <filters/ldpc_decoder.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <filters/fft.hpp>
#include <utils/utils.hpp>

static constexpr size_t UPSAMPLE_FACTOR = 1;
static constexpr size_t MOD_TICKS_PER_SYMBOL = 3;
//ticks per symbol needs to be greater than upsample factor in order to not saturate
static constexpr size_t MIXER_TICKS_PER_PERIOD = (MOD_TICKS_PER_SYMBOL / UPSAMPLE_FACTOR) * 1;

static void construct_tx_chain(FilterChain &txChain, Mapper::constellation_set_t scheme)
{
    Mapper * qam16    = new Mapper(MOD_TICKS_PER_SYMBOL, scheme);
    Mixer *     upmixer  = new Mixer(MIXER_TICKS_PER_PERIOD, true /* upmix */);
    txChain = *upmixer + *qam16;
}

static void construct_rx_chain(FilterChain &rxChain, Mapper::constellation_set_t scheme)
{
    Mixer *downmixer = new Mixer(MIXER_TICKS_PER_PERIOD, false /* downmix */);
    Decimator *decim = new Decimator(MOD_TICKS_PER_SYMBOL, 0);
    Demapper *demod = new Demapper(scheme, true);
    NoiseElement * ne = new NoiseElement(15);
    rxChain = *demod + *decim + *downmixer + *ne;
}

static void construct_ldpc_ebn0_tx(FilterChain &txChain)
{
    CSVBitMatrix p;
    std::vector<char> g_bytes = p.loadCSVFile("data/ldpc/80211n_G_648_12.csv");
    std::vector<std::vector<bool> > G;
    p.parseCSV(g_bytes, G);

    LDPCEncode * encode  = new LDPCEncode(G);
    Mapper *     mapper    = new Mapper(1, Mapper::CONST_SET_BPSK);

    txChain = *mapper + *encode;
}

static void construct_ldpc_enb0_rx(FilterChain &rxChain, double ebn0)
{
    CSVBitMatrix p;
    std::vector<char> bytes = p.loadCSVFile("data/ldpc/80211n_H_648_12.csv");
    std::vector<std::vector<bool> > H;
    p.parseCSV(bytes, H);

    LDPCDecoder * decode = new LDPCDecoder(H);
    Demapper * demapper = new Demapper(Mapper::CONST_SET_BPSK, false);
    NoiseElement * ne    = new NoiseElement(ebn0);

    rxChain = *decode + *demapper + *ne;
}

void construct_radio_set(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, Mapper::constellation_set_t scheme)
{
    size_t count = 0;
    for (auto it = coords.begin(); it != coords.end(); it++) {
        rs.addRadio([scheme, it, count]()
                {
                    radio_config_t config {
                        .position = Vector2d(it->first, it->second),
                        .id = static_cast<radio_id_t>(count)
                    };
                    std::cout << "Adding radio with id = " << count << std::endl;

                    FilterChain modulation_chain;
                    construct_tx_chain(modulation_chain, scheme);

                    FilterChain demodulation_chain;
                    construct_rx_chain(demodulation_chain, scheme);

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                });
        count++;
    }
    rs.init(false, true, true);
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

