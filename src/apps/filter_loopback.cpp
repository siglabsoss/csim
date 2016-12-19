#include <core/filter_chain.hpp>
#include <core/logger.hpp>

#include <types/circularbuffer.hpp>

#include <filters/fft.hpp>
#include <filters/demapper.hpp>
#include <filters/mapper.hpp>
#include <filters/noise_element.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <filters/cyclic_prefix.hpp>
#include <filters/frame_sync.hpp>
#include <filters/channel_equalizer.hpp>
#include <filters/ldpc_encode.hpp>
#include <filters/ldpc_decoder.hpp>
#include <filters/puncture.hpp>
#include <filters/depuncture.hpp>
#include <filters/scrambler_block.hpp>

#include <utils/utils.hpp>
#include <utils/ldpc_utils.hpp>

#include <probes/sample_count_trigger.hpp>

#include <3rd/json/json.h>

static constexpr double MIXER_FREQ = 0.16;
static constexpr size_t FFT_SIZE = 1024;
static constexpr size_t CP_SIZE = 100;
static constexpr size_t DUC_UPSAMPLE_FACTOR = 10;

#undef SHOULD_PROBE_FILTERS

static size_t runFilters(FilterChain &chain, size_t numBits, size_t &numBitErrors, size_t maxBitErr, size_t outputsPerPrint)
{
    filter_io_t sample;
    size_t outputCount = 0;
    size_t printCount = 0;
    numBitErrors = 0;

    while (outputCount < numBits) {
        if (chain.getFIFOUtilization() < 0.5) {
            sample.type = IO_TYPE_BIT;
            sample.bit = 1; //we can always use '1' because the scrambler will produce equal number of '1's and '0's
            chain.input(sample);
        }

        for (size_t i = 0; i < 10; ++i) {
            chain.tick();
            if (chain.output(sample)) {
                assert(sample.type == IO_TYPE_BIT);
                outputCount++;
                if (1 != sample.bit) {
                    numBitErrors++;
                    if (numBitErrors >=  maxBitErr) {
                        return outputCount;
                    }
                }
                if (++printCount >= outputsPerPrint) {
                    std::cout << outputCount << " bits sent with " << numBitErrors << " errors." << std::endl;
                    printCount = 0;
                }
            }
        }
    }
    return outputCount;
}

static FilterChain constructLoopbackChain(double noiseVar, size_t syncDelay, const MCS mcs, const std::string &down2CoeffFile, const std::string &down5CoeffFile,
        const std::string &up2CoeffFile, const std::string &up5CoeffFile, const std::string &HfFile,
        const std::string &ldpcH, const std::string &ldpcG)
{
    // Prepare coefficients
    std::vector<ComplexDouble> halfbandComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>(down2CoeffFile);
    std::vector<ComplexDouble> by5ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(down5CoeffFile);
    std::vector<ComplexDouble> up2ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(up2CoeffFile);
    std::vector<ComplexDouble> up5ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(up5CoeffFile);
    assert(halfbandComplexCoeffs.size() != 0 && by5ComplexCoeffs.size() != 0);
    assert(up2CoeffFile.size() != 0 && up5CoeffFile.size() != 0);

    std::vector<double> down2Coeffs(halfbandComplexCoeffs.size());
    std::vector<double> down5Coeffs(by5ComplexCoeffs.size());
    std::vector<double> up2Coeffs(up2ComplexCoeffs.size());
    std::vector<double> up5Coeffs(up5ComplexCoeffs.size());

    for (size_t i = 0; i < halfbandComplexCoeffs.size(); i++) {
        down2Coeffs[i] = halfbandComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < by5ComplexCoeffs.size(); i++) {
        down5Coeffs[i] = by5ComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < up2ComplexCoeffs.size(); i++) {
        up2Coeffs[i] = up2ComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < up5ComplexCoeffs.size(); i++) {
        up5Coeffs[i] = up5ComplexCoeffs[i].real();
    }

    //Channel equalization (calculated / exported from MATLAB)
    std::vector<ComplexDouble> Hf = utils::readComplexFromCSV<ComplexDouble>(HfFile);

    //LDPC Matrices
    std::vector<std::vector<bool> > G = LDPCUtils::getBitArrayFromCSV(ldpcG);
    std::vector<std::vector<bool> > H = LDPCUtils::getBitArrayFromCSV(ldpcH);

    //Initialize blocks
    ScramblerBlock *scramTx     = new ScramblerBlock;
    LDPCEncode * encode         = new LDPCEncode(G);
    Puncture *punc              = new Puncture(mcs);
    Mapper * mapper             = new Mapper(mcs);
    FFT * ifft                  = new FFT(FFT_SIZE, true);
    ifft->setOutputFormat(FFT_OUTPUT_WL, 1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    CyclicPrefix *cp            = new CyclicPrefix(FFT_SIZE, CP_SIZE, DUC_UPSAMPLE_FACTOR, mcs);
    DigitalUpConverter * duc    = new DigitalUpConverter(-MIXER_FREQ, up2Coeffs, up5Coeffs);

    ScramblerBlock *scramRx     = new ScramblerBlock;
    LDPCDecoder * decode        = new LDPCDecoder(H);
    Demapper * demapper         = new Demapper(mcs);
    FFT * fft                   = new FFT(FFT_SIZE, false);
    fft->setOutputFormat(FFT_OUTPUT_WL, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    DigitalDownConverter *ddc   = new DigitalDownConverter(MIXER_FREQ, down2Coeffs, down5Coeffs);
    FrameSync    *fs            = new FrameSync(FFT_SIZE, CP_SIZE, syncDelay);
    ChannelEqualizer *ce        = new ChannelEqualizer(Hf);
    Depuncture *depunc          = new Depuncture(mcs);

    NoiseElement *ne            = new NoiseElement(noiseVar);

#ifdef SHOULD_PROBE_FILTERS
    //Debug probes
    std::string duc_in_probe_name   = "DUC_INPUT";
    std::string ddc_out_probe_name  = "DDC_OUTPUT";
    std::string ifft_in_probe_name  = "IFFT_INPUT";
    std::string ifft_out_probe_name = "IFFT_OUTPUT";
    std::string fft_out_probe_name  = "FFT_OUTPUT";
    std::string fft_in_probe_name   = "FFT_INPUT";
    constexpr size_t PHASE_DELAY    = 40; //found emperically
    constexpr size_t NUM_FRAMES_TO_CAPTURE = 7;
    SampleCountTrigger *duc_in      = new SampleCountTrigger(duc_in_probe_name,   FilterProbe::CSV, (FFT_SIZE + CP_SIZE)*NUM_FRAMES_TO_CAPTURE, 1, 0);
    SampleCountTrigger *ddc_out     = new SampleCountTrigger(ddc_out_probe_name,  FilterProbe::CSV, (FFT_SIZE + CP_SIZE)*NUM_FRAMES_TO_CAPTURE + PHASE_DELAY, 1, 0);
    SampleCountTrigger *ifft_in     = new SampleCountTrigger(ifft_in_probe_name,  FilterProbe::CSV, FFT_SIZE*NUM_FRAMES_TO_CAPTURE, 1, 0);
    SampleCountTrigger *ifft_out    = new SampleCountTrigger(ifft_out_probe_name, FilterProbe::CSV, FFT_SIZE*NUM_FRAMES_TO_CAPTURE, 1, 0);
    SampleCountTrigger *fft_out     = new SampleCountTrigger(fft_out_probe_name,  FilterProbe::CSV, FFT_SIZE*NUM_FRAMES_TO_CAPTURE, 1, 0);
    SampleCountTrigger *fft_in      = new SampleCountTrigger(fft_in_probe_name,   FilterProbe::CSV, FFT_SIZE*NUM_FRAMES_TO_CAPTURE, 1, 0);

    FilterChain testChain =  *scramRx + *decode + *depunc + *demapper + *fft_out + *ce + *fft + *fft_in + *fs + *ddc_out + *ddc + /* *ne + */ *duc + *duc_in + *cp + *ifft_out + *ifft + *ifft_in + *mapper + *punc + *encode + *scramTx;
#else
    FilterChain testChain =  *scramRx + *decode + *depunc + *demapper + *ce + *fft + *fs + *ddc + *ne + *duc + *cp + *ifft + *mapper + *punc + *encode + *scramTx;
#endif

    return testChain;
}

static double getNoiseVarFromSNR(double snr)
{
    //determine noise power from desired signal to noise ratio
    //some relevant info here: http://read.pudn.com/downloads152/doc/comm/664022/ber.pdf
    return (1.0 / 1024.0) / (pow(10, snr/10.0));
}

static bool validateConfiguration(const Json::Value &config)
{
    if (!config.isMember("ldpc")    ||
            !config.isMember("ddc") ||
            !config.isMember("duc") ||
            !config.isMember("snr") ||
            !config.isMember("Hf")  ||
            !config.isMember("sync") ||
            !config.isMember("plot")
       )
    {
        return false;
    }
    const Json::Value &ldpc = config["ldpc"];
    const Json::Value &ddc  = config["ddc"];
    const Json::Value &duc  = config["duc"];
    const Json::Value &snr  = config["snr"];
    const Json::Value &sync  = config["sync"];
    const Json::Value &plot  = config["plot"];

    if (!ldpc.isMember("H") || !ldpc.isMember("G")) {
        return false;
    }

    if (!ldpc["H"].isString() || !ldpc["G"].isString()) {
        return false;
    }

    if (!ddc.isMember("down2") || !ddc.isMember("down5")) {
        return false;
    }

    if (!ddc["down2"].isString() || !ddc["down5"].isString()) {
        return false;
    }

    if (!duc.isMember("up2") || !duc.isMember("up5")) {
        return false;
    }

    if (!duc["up2"].isString() || !duc["up5"].isString()) {
        return false;
    }

    if (!config["Hf"].isString()) {
        return false;
    }

    if (!snr.isMember("start")          ||
            !snr.isMember("end")        ||
            !snr.isMember("transition") ||
            !snr.isMember("fine_incr")  ||
            !snr.isMember("coarse_incr") ||
            !snr.isMember("max_bit_err") ||
            !snr.isMember("outputs_per_print") ||
            !snr.isMember("num_frames_to_send") ||
            !snr.isMember("frame_size")
            ) {
        return false;
    }

    if (!snr["start"].isDouble()          ||
            !snr["end"].isDouble()        ||
            !snr["transition"].isDouble() ||
            !snr["fine_incr"].isDouble()  ||
            !snr["coarse_incr"].isDouble() ||
            !snr["max_bit_err"].isIntegral() ||
            !snr["outputs_per_print"].isIntegral() ||
            !snr["num_frames_to_send"].isIntegral() ||
            !snr["frame_size"].isIntegral()
    )
    {
        return false;
    }

    if (!sync.isMember("delay")) {
        return false;
    }

    if (!sync["delay"].isIntegral()) {
        return false;
    }

    if (!plot.isMember("title") || !plot["title"].isString()) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
        return 1;
    }

    std::srand(1473294057+1);

    Json::Value config;
    Json::Reader configReader;

    std::ifstream stream;
    stream.open(argv[1], std::ifstream::in);

    if (stream.is_open() == false) {
        std::cout << "Error opening configuration file!" << std::endl;
        return 1;
    }

    if (configReader.parse(stream, config) == false) {
        std::cout << "Error reading configuration file!" << std::endl;
        return 1;
    }

    if (validateConfiguration(config) == false) {
        std::cout << "Invalid configuration file!" << std::endl;
        return 1;
    }
    const Json::Value &ldpc = config["ldpc"];
    const Json::Value &ddc  = config["ddc"];
    const Json::Value &duc  = config["duc"];
    const Json::Value &snr  = config["snr"];
    const Json::Value &sync  = config["sync"];
    const Json::Value &plotcfg  = config["plot"];

    std::string down2 = ddc["down2"].asString();
    std::string down5 = ddc["down5"].asString();
    std::string up2 = duc["up2"].asString();
    std::string up5 = duc["up5"].asString();
    std::string Hf = config["Hf"].asString();
    std::string ldpcH = ldpc["H"].asString();
    std::string ldpcG = ldpc["G"].asString();
    std::string title = plotcfg["title"].asString();

    const plotter &plot = plotter::get();
    std::vector<std::vector<double> > bers(1);
    std::vector<std::vector<double> > ebnos(1);
    std::vector<std::string> titles;
    titles.push_back("OFDM Loopback");

    //This frame size passed to the MCS object is chosen to avoid 0 padding
    const size_t NUM_FRAMES_TO_TRANSMIT  = snr["num_frames_to_send"].asInt();
    const size_t FRAME_SIZE              = snr["frame_size"].asInt();
    const double SNR_START               = snr["start"].asDouble();
    const double SNR_END                 = snr["end"].asDouble();
    const double SNR_COARSE_TRANSITION   = snr["transition"].asDouble();
    const double SNR_COARSE_INCREMENT    = snr["coarse_incr"].asDouble();
    const double SNR_FINE_INCREMENT      = snr["fine_incr"].asDouble();
    const size_t MAX_BIT_ERRORS          = snr["max_bit_err"].asInt();
    const size_t OUTPUTS_PER_PRINT       = snr["outputs_per_print"].asInt();

    const size_t FRAME_SYNC_DELAY        = sync["delay"].asInt();

    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, FRAME_SIZE, FFT_SIZE);

    std::cout << "Testing SNR of " << SNR_START << " to " << SNR_END << " with coarse increments of " << SNR_COARSE_INCREMENT << " and fine incrememnts of " << SNR_FINE_INCREMENT << std::endl;
    std::cout << "Counting up to " << MAX_BIT_ERRORS << " bit errors per SNR or " << NUM_FRAMES_TO_TRANSMIT << " frames, " << FRAME_SIZE << " bits long" << std::endl;

    double snrVal = SNR_START;
    while (snrVal <= SNR_END) {
        std::cout << "Starting run for SNR of " << snrVal << std::endl;
        double noiseVar = getNoiseVarFromSNR(snrVal);
        FilterChain loopback = constructLoopbackChain(noiseVar, FRAME_SYNC_DELAY, mcs, down2, down5, up2, up5, Hf, ldpcH, ldpcG);

        size_t numBitErrors;
        size_t numBitsOutput = runFilters(loopback, FRAME_SIZE*NUM_FRAMES_TO_TRANSMIT, numBitErrors, MAX_BIT_ERRORS, OUTPUTS_PER_PRINT);
        double ber = static_cast<double>(numBitErrors) / static_cast<double>(numBitsOutput);
        bers[0].push_back(ber);
        ebnos[0].push_back(snrVal);
        if (numBitsOutput > 0) {
            std::cout << "SNR," << snrVal << ",ERR," << numBitErrors << ",OUT," << numBitsOutput << ",BER," << ber << std::endl;
        } else {
            std::cout << "No bits were output" << std::endl;
        }

        if (ber == 0.0) {
            std::cout << "Did not see any bit errors. Stopping." << std::endl;
            break;
        }
        if (snrVal < SNR_COARSE_TRANSITION) {
            snrVal += SNR_COARSE_INCREMENT;
        } else {
            snrVal += SNR_FINE_INCREMENT;
        }
    }

    plot.nplotmulti(bers, ebnos, titles, "SNR (dB)", "BER", title, true);
    sleep(1);

    return 0;
}
