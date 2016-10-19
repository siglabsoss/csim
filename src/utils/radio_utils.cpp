#include <utils/radio_utils.hpp>

#include <filters/mixer.hpp>
#include <filters/decimator.hpp>
#include <filters/hard_demod.hpp>
#include <filters/noise_element.hpp>


static constexpr size_t UPSAMPLE_FACTOR = 1;
static constexpr size_t MOD_TICKS_PER_SYMBOL = 3;
//ticks per symbol needs to be greater than upsample factor in order to not saturate
static constexpr size_t MIXER_TICKS_PER_PERIOD = (MOD_TICKS_PER_SYMBOL / UPSAMPLE_FACTOR) * 1;

static void construct_tx_chain(FilterChain &modChain, Mapper::constellation_set_t scheme)
{
    Mapper * qam16    = new Mapper(MOD_TICKS_PER_SYMBOL, scheme);
    Mixer *     upmixer  = new Mixer(MIXER_TICKS_PER_PERIOD, true /* upmix */);
    modChain = *upmixer + *qam16;
}

static void construct_rx_chain(FilterChain &demodChain, Mapper::constellation_set_t scheme)
{
    Mixer *downmixer = new Mixer(MIXER_TICKS_PER_PERIOD, false /* downmix */);
    Decimator *decim = new Decimator(MOD_TICKS_PER_SYMBOL, 0);
    HardDemod *demod = new HardDemod(scheme, 0.0);
    NoiseElement * ne = new NoiseElement(15);
    demodChain = *demod + *decim + *downmixer + *ne;
}

static void construct_rx_chain_ebn0(FilterChain &demodChain, Mapper::constellation_set_t scheme, double ebn0)
{
    Mixer *downmixer = new Mixer(MIXER_TICKS_PER_PERIOD, false /* downmix */);
    Decimator *decim = new Decimator(MOD_TICKS_PER_SYMBOL, 0);
    HardDemod *demod = new HardDemod(scheme, 0.0);
    __attribute__ ((unused)) NoiseElement * ne = new NoiseElement(ebn0);
//    demodChain = *demod + *decim + *downmixer;
    demodChain = *demod + *decim + *downmixer + *ne;
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

void construct_radio_set_ebn0(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, double ebn0, Mapper::constellation_set_t scheme)
{
    size_t count = 0;
    for (auto it = coords.begin(); it != coords.end(); it++) {
        rs.addRadio([scheme, it, count, ebn0]()
                {
                    radio_config_t config {
                        .position = Vector2d(it->first, it->second),
                        .id = static_cast<radio_id_t>(count)
                    };
                    std::cout << "Adding radio with id = " << count << std::endl;

                    FilterChain modulation_chain;
                    construct_tx_chain(modulation_chain, scheme);

                    FilterChain demodulation_chain;
                    construct_rx_chain_ebn0(demodulation_chain, scheme, ebn0);

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                });
        count++;
    }
    rs.init(false, true, true);
}
