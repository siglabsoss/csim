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

static constexpr double MIXER_FREQ = 0.16;
static constexpr size_t FFT_SIZE = 1024;
static constexpr size_t CP_SIZE = 100;
static constexpr size_t DUC_UPSAMPLE_FACTOR = 10;
static constexpr size_t OUTPUTS_PER_PRINT_STATEMENT = 10000;

#undef SHOULD_PROBE_FILTERS

static size_t runFilters(FilterChain &chain, size_t numBits, size_t &numBitErrors)
{
    static constexpr size_t MAX_BIT_ERRORS = 4000;
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
                    if (numBitErrors >=  MAX_BIT_ERRORS) {
                        return outputCount;
                    }
                }
                if (++printCount >= OUTPUTS_PER_PRINT_STATEMENT) {
                    std::cout << outputCount << " bits sent with " << numBitErrors << " errors." << std::endl;
                    printCount = 0;
                }
            }
        }
    }
    return outputCount;
}

static FilterChain constructLoopbackChain(double noiseVar, const MCS mcs, const std::string &down2CoeffFile, const std::string &down5CoeffFile,
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
    FrameSync    *fs            = new FrameSync(FFT_SIZE, CP_SIZE);
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

int main(int argc, char *argv[])
{
    if (argc != 8) {
        std::cout << "Usage: " << argv[0] << " <ddc_down2_coeffs> <ddc_down5_coeffs> <duc_up2_coeffs> <duc_up5_coeffs> <Hf_file> <ldpc H> <ldpc G>" << std::endl;
        return 1;
    }

    std::srand(1473294057+1);
    std::string down2(argv[1]);
    std::string down5(argv[2]);
    std::string up2(argv[3]);
    std::string up5(argv[4]);
    std::string Hf(argv[5]);
    std::string ldpcH(argv[6]);
    std::string ldpcG(argv[7]);

    const plotter &plot = plotter::get();
    std::vector<std::vector<double> > bers(1);
    std::vector<std::vector<double> > ebnos(1);
    std::vector<std::string> titles;
    titles.push_back("OFDM Loopback");

    //This needs to change with the LDPC codeword specified from the command line arguments.
    //Obviously hard coding this here is not ideal but we'll get by for now.
    constexpr size_t EXPECTED_CODEWORD_LENGTH = 1944;
    //This frame size passed to the MCS object is chosen to avoid 0 padding
    constexpr size_t FRAME_SIZE = (EXPECTED_CODEWORD_LENGTH / 2)*4;
    constexpr size_t NUM_FRAMES_TO_TRANSMIT  = 220;
    constexpr double SNR_START               = -15.0;
    constexpr double SNR_END                 = -5.0;
    constexpr double SNR_COARSE_TRANSITION   = -10.0;
    constexpr double SNR_COARSE_INCREMENT    = 1.0;
    constexpr double SNR_FINE_INCREMENT      = 0.25;

    MCS mcs(MCS::ONE_HALF_RATE, MCS::MOD_BPSK, FRAME_SIZE, FFT_SIZE);

    double snr = SNR_START;
    while (snr <= SNR_END) {
        std::cout << "Starting run for SNR of " << snr << std::endl;
        double noiseVar = getNoiseVarFromSNR(snr);
        FilterChain loopback = constructLoopbackChain(noiseVar, mcs, down2, down5, up2, up5, Hf, ldpcH, ldpcG);

        size_t numBitErrors;
        size_t numBitsOutput = runFilters(loopback, FRAME_SIZE*NUM_FRAMES_TO_TRANSMIT, numBitErrors);
        double ber = static_cast<double>(numBitErrors) / static_cast<double>(numBitsOutput);
        bers[0].push_back(ber);
        ebnos[0].push_back(snr);
        if (numBitsOutput > 0) {
            std::cout << "SNR," << snr << ",ERR," << numBitErrors << ",OUT," << numBitsOutput << ",BER," << ber << std::endl;
        } else {
            std::cout << "No bits were output" << std::endl;
        }

        if (ber == 0.0) {
            std::cout << "Did not see any bit errors. Stopping." << std::endl;
            break;
        }
        if (snr < SNR_COARSE_TRANSITION) {
            snr += SNR_COARSE_INCREMENT;
        } else {
            snr += SNR_FINE_INCREMENT;
        }
    }

    plot.nplotmulti(bers, ebnos, titles, "SNR (dB)", "BER", "OFDM Loopback (LDPC 802.11n code 1944 bit 1/2 rate)", true);
    sleep(1);

    return 0;
}
