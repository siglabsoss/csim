#include <utils/radio_utils.hpp>
#include <utils/ldpc_utils.hpp>

#include <filters/mixer.hpp>
#include <filters/decimator.hpp>
#include <filters/hard_demapper.hpp>
#include <filters/soft_demapper.hpp>
#include <filters/noise_element.hpp>
#include <filters/ldpc_encode.hpp>
#include <filters/ldpc_decoder.hpp>
#include <filters/ddc.hpp>
#include <filters/duc.hpp>
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
    HardDemapper *demod = new HardDemapper(scheme, 0.0);
    NoiseElement * ne = new NoiseElement(15);
    rxChain = *demod + *decim + *downmixer + *ne;
}

static void construct_rx_chain_ebn0(FilterChain &rxChain, Mapper::constellation_set_t scheme, double ebn0)
{
    Mixer *downmixer = new Mixer(MIXER_TICKS_PER_PERIOD, false /* downmix */);
    Decimator *decim = new Decimator(MOD_TICKS_PER_SYMBOL, 0);
    HardDemapper *demod = new HardDemapper(scheme, 0.0);
    NoiseElement * ne = new NoiseElement(ebn0);
//    rxChain = *demod + *decim + *downmixer;
    rxChain = *demod + *decim + *downmixer + *ne;
}

static void construct_ldpc_ebn0_tx(FilterChain &txChain)
{
    CSVBitMatrix p;
    std::vector<char> g_bytes = p.loadCSVFile("data/ldpc/80211n_G.csv");
    std::vector<std::vector<bool> > G;
    p.parseCSV(g_bytes, G);

    LDPCEncode * encode  = new LDPCEncode(G);
    Mapper *     bpsk    = new Mapper(1, Mapper::CONST_SET_BPSK);

    txChain = *bpsk + *encode;
}

static void construct_ldpc_enb0_rx(FilterChain &rxChain, double ebn0)
{
    CSVBitMatrix p;
    std::vector<char> bytes = p.loadCSVFile("data/ldpc/80211n_H.csv");
    std::vector<std::vector<bool> > H;
    p.parseCSV(bytes, H);

    LDPCDecoder * decode = new LDPCDecoder(H);
    SoftDemapper * demapper = new SoftDemapper(Mapper::CONST_SET_BPSK);
    NoiseElement * ne    = new NoiseElement(ebn0);

    rxChain = *decode + *demapper + *ne;
}
static constexpr double DDC_DUC_FREQ = 0.16;
static void construct_loopback_tx(FilterChain &txChain)
{
    CSVBitMatrix p;
    std::vector<char> g_bytes = p.loadCSVFile("data/ldpc/80211n_G.csv");
    std::vector<std::vector<bool> > G;
    p.parseCSV(g_bytes, G);

    std::vector<ComplexDouble> up2ComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>("./data/ddc/coeffs/halfband_interp.txt");
    std::vector<ComplexDouble> up5ComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>("./data/ddc/coeffs/by5_interp.txt");
    assert(up2ComplexCoeffs.size() > 0);
    assert(up5ComplexCoeffs.size() > 0);

    std::vector<double> up2Coeffs(up2ComplexCoeffs.size());
    std::vector<double> up5Coeffs(up5ComplexCoeffs.size());

    for (size_t i = 0; i < up2ComplexCoeffs.size(); i++) {
        up2Coeffs[i] = up2ComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < up5ComplexCoeffs.size(); i++) {
        up5Coeffs[i] = up5ComplexCoeffs[i].real();
    }

    DigitalUpConverter * duc = new DigitalUpConverter(DDC_DUC_FREQ, up2Coeffs, up5Coeffs);
    LDPCEncode * encode      = new LDPCEncode(G);
    Mapper * bpsk            = new Mapper(10, Mapper::CONST_SET_BPSK);

    txChain = *duc + *bpsk + *encode;
}

static void construct_loopback_rx(FilterChain &rxChain, double ebn0)
{
    CSVBitMatrix p;
    std::vector<char> bytes = p.loadCSVFile("data/ldpc/80211n_H.csv");
    std::vector<std::vector<bool> > H;
    p.parseCSV(bytes, H);

    std::vector<ComplexDouble> halfbandComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>("./data/ddc/coeffs/halfband.txt");
    std::vector<ComplexDouble> by5ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>("./data/ddc/coeffs/downby5.txt");
    assert(halfbandComplexCoeffs.size() > 0);
    assert(by5ComplexCoeffs.size() > 0);

    std::vector<double> halfbandCoeffs(halfbandComplexCoeffs.size());
    std::vector<double> by5Coeffs(by5ComplexCoeffs.size());

    for (size_t i = 0; i < halfbandComplexCoeffs.size(); i++) {
        halfbandCoeffs[i] = halfbandComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < by5ComplexCoeffs.size(); i++) {
        by5Coeffs[i] = by5ComplexCoeffs[i].real();
    }

    LDPCDecoder * decode = new LDPCDecoder(H);
    SoftDemapper * demapper = new SoftDemapper(Mapper::CONST_SET_BPSK);
    NoiseElement * ne    = new NoiseElement(ebn0);
    DigitalDownConverter * ddc = new DigitalDownConverter(DDC_DUC_FREQ, halfbandCoeffs, by5Coeffs);

    rxChain = *decode + *demapper + *ddc + *ne;
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

void construct_radio_set_loopback_ebn0(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, double ebn0)
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
                    construct_loopback_tx(modulation_chain);

                    FilterChain demodulation_chain;
                    construct_loopback_rx(demodulation_chain, ebn0);

                    return std::unique_ptr<RadioS>(new RadioS(config, modulation_chain, demodulation_chain));
                });
        count++;
    }
    rs.init(true, true, true); //no noise, no delay, no phase rotation
}
