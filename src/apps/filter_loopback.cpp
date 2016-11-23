#include <core/filter_chain.hpp>
#include <core/logger.hpp>

#include <filters/fft.hpp>
#include <filters/demapper.hpp>
#include <filters/mapper.hpp>
#include <filters/noise_element.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <filters/cyclic_prefix.hpp>
#include <filters/frame_sync.hpp>

#include <utils/utils.hpp>

#include <probes/sample_count_trigger.hpp>

static constexpr double MIXER_FREQ = 0.16;
static constexpr size_t FFT_SIZE = 1024;
static constexpr size_t CP_SIZE = 100;
static constexpr size_t NUM_INPUT_BITS = FFT_SIZE * 1;
static constexpr size_t UPSAMPLE_FACTOR = 10;

static void loadInput(std::vector<bool> &inputs)
{
    for (size_t i = 0; i < inputs.size(); i++) {
        bool bit = static_cast<bool>(std::rand() & 1);
        inputs[i] = bit;
    }
}

static void checkOutputs(const std::vector<bool> &inputs, const std::vector<bool> &outputs)
{
    size_t numCorrect = 0;
    size_t numIncorrect = 0;
    for (size_t i = 0; i < outputs.size(); i++) {
        if (inputs[(i)] != outputs[i]) {
            std::cout << "#" << i << " " << inputs[(i)] << " != " << outputs[i] << std::endl;
            numIncorrect++;
        } else {
            numCorrect++;
        }
//        std::cout << inputs[(i)] << "," << outputs[i] << std::endl;
    }

    std::cout << "Bit error rate = " << static_cast<double>(numIncorrect) / static_cast<double>(numIncorrect + numCorrect) << std::endl;
}

//static void printInputsAndOutputs(const std::vector<bool> &inputs, const std::vector<bool> &outputs)
//{
//    assert(inputs.size() == outputs.size());
//    for (size_t i = 0; i < inputs.size(); i++) {
//        std::cout << inputs[i] << "," << outputs[i] << std::endl;
//    }
//}

static size_t runFilters(FilterChain &chain, const std::vector<bool> &inputs, std::vector<bool> &outputs)
{
    filter_io_t sample;
    size_t inputCount = 0;
    while (outputs.size() < inputs.size()) {
        sample.type = IO_TYPE_BIT;
        sample.bit = inputs[inputCount];
        if (chain.input(sample)) {
            inputCount++;//only move on to the next sample if our sample was accepted
        }
        chain.tick();
        if (chain.output(sample)) {
            assert(sample.type == IO_TYPE_BIT);
            outputs.push_back(sample.bit);
        }
    }
    return outputs.size();
}

static void symbolMappingFFTDUCLoopback(const std::string &down2CoeffFile, const std::string &down5CoeffFile,
        const std::string &up2CoeffFile, const std::string &up5CoeffFile,
        const std::vector<bool> &inputs, std::vector<bool> &outputs)
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

    //Initialize blocks
    Mapper * mapper             = new Mapper(UPSAMPLE_FACTOR*2, Mapper::CONST_SET_BPSK);
    FFT * ifft                  = new FFT(FFT_SIZE, true, UPSAMPLE_FACTOR*2);
    ifft->setOutputFormat(FFT_OUTPUT_WL, 1, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    CyclicPrefix *cp            = new CyclicPrefix(FFT_SIZE, CP_SIZE, UPSAMPLE_FACTOR);
    DigitalUpConverter * duc    = new DigitalUpConverter(-MIXER_FREQ, up2Coeffs, up5Coeffs);


    Demapper * demapper         = new Demapper(Mapper::CONST_SET_BPSK, true);
    FFT * fft                   = new FFT(FFT_SIZE, false, UPSAMPLE_FACTOR);
    fft->setOutputFormat(FFT_OUTPUT_WL, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    DigitalDownConverter *ddc   = new DigitalDownConverter(MIXER_FREQ, down2Coeffs, down5Coeffs);
    FrameSync    *fs                = new FrameSync(FFT_SIZE, CP_SIZE);

    //Debug probes
    std::string duc_in_probe_name   = "DUC_INPUT";
    std::string ddc_out_probe_name  = "DDC_OUTPUT";
    std::string ifft_in_probe_name  = "IFFT_INPUT";
    std::string ifft_out_probe_name = "IFFT_OUTPUT";
    std::string fft_out_probe_name  = "FFT_OUTPUT";
    std::string fft_in_probe_name  = "FFT_INPUT";
    constexpr size_t PHASE_DELAY = 40; //found emperically
    SampleCountTrigger *duc_in      = new SampleCountTrigger(duc_in_probe_name,   FilterProbe::CSV, (FFT_SIZE + CP_SIZE)*1, 1, 0);
    SampleCountTrigger *ddc_out     = new SampleCountTrigger(ddc_out_probe_name,  FilterProbe::CSV, (FFT_SIZE + CP_SIZE)*1 + PHASE_DELAY, 1, 0);
    SampleCountTrigger *ifft_in     = new SampleCountTrigger(ifft_in_probe_name,  FilterProbe::CSV, FFT_SIZE*1, 1, 0);
    SampleCountTrigger *ifft_out    = new SampleCountTrigger(ifft_out_probe_name, FilterProbe::CSV, FFT_SIZE*1, 1, 0);
    SampleCountTrigger *fft_out     = new SampleCountTrigger(fft_out_probe_name,  FilterProbe::CSV, FFT_SIZE*1, 1, 0);
    SampleCountTrigger *fft_in      = new SampleCountTrigger(fft_in_probe_name,   FilterProbe::CSV, FFT_SIZE*1, 1, 0);

    //Test symbol mapping + FFT + DUC
    FilterChain testChain =  *demapper + *fft_out + *fft + *fft_in + *fs + *ddc_out + *ddc + *duc + *duc_in + *cp + *ifft_out + *ifft + *ifft_in + *mapper;

    size_t numOutputs = runFilters(testChain, inputs, outputs);
    std::cout << "FFT/DUC Loopback has " << numOutputs << " outputs" << std::endl;

    checkOutputs(inputs, outputs);
//    printInputsAndOutputs(inputs, outputs);
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <ddc_down2_coeffs> <ddc_down5_coeffs> <duc_up2_coeffs> <duc_up5_coeffs>" << std::endl;
        return 1;
    }

    std::srand(1473294057+1);
    std::string down2(argv[1]);
    std::string down5(argv[2]);
    std::string up2(argv[3]);
    std::string up5(argv[4]);

    std::vector<bool> inputs(NUM_INPUT_BITS);
    std::vector<bool> outputs;

    //Calculate random inputs
    loadInput(inputs);

    symbolMappingFFTDUCLoopback(down2, down5, up2, up5, inputs, outputs);

    return 0;
}
