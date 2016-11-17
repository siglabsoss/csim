#include <core/filter_chain.hpp>
#include <core/logger.hpp>

#include <filters/fft.hpp>
#include <filters/demapper.hpp>
#include <filters/mapper.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <filters/ddc_duc/ddc.hpp>

#include <utils/utils.hpp>

#include <probes/sample_count_trigger.hpp>

static constexpr double MIXER_FREQ = 0.16;
static constexpr size_t FFT_SIZE = 1024;
static constexpr size_t NUM_INPUT_BITS = FFT_SIZE * 4;
static constexpr size_t UPSAMPLE_FACTOR = 10;

static void loadInput(std::vector<bool> &inputs)
{
    for (size_t i = 0; i < inputs.size(); i++) {
        bool bit = static_cast<bool>(std::rand() & 1);
        inputs[i] = bit;
    }
}

static void checkOutputs(const std::vector<bool> &inputs, const std::vector<bool> &outputs, size_t outputLag, size_t inputUpsample)
{
    assert(inputUpsample > 0);
    size_t numCorrect = 0;
    size_t numIncorrect = 0;
    for (size_t i = 0; i < outputs.size(); i++) {
        if (i >= outputLag) {
            if (inputs[(i-outputLag)/inputUpsample] != outputs[i]) {
                std::cout << "#" << i << " " << inputs[(i-outputLag)/inputUpsample] << " != " << outputs[i] << std::endl;
                numIncorrect++;
            } else {
                numCorrect++;
            }
            std::cout << inputs[(i-outputLag)/inputUpsample] << "," << outputs[i] << std::endl;
        }
    }

    std::cout << "Bit error rate = " << static_cast<double>(numIncorrect) / static_cast<double>(numIncorrect + numCorrect) << std::endl;
}

static size_t runFilters(FilterChain &chain, const std::vector<bool> &inputs, std::vector<bool> &outputs, size_t inputUpsample)
{
    assert(inputUpsample > 0);
    filter_io_t sample;
    for (size_t i = 0; i < inputs.size(); ++i) {
        sample.type = IO_TYPE_BIT;
        for (size_t k = 0; k < inputUpsample; k++) {
            sample.bit = inputs[i];
            assert(chain.input(sample));
            for (size_t j = 0; j < UPSAMPLE_FACTOR; ++j) {
                chain.tick();
                if (chain.output(sample)) {
                    assert(sample.type == IO_TYPE_BIT);
                    outputs.push_back(sample.bit);
                }
            }
        }
    }
    return outputs.size();
}

//static void symbolMappingLoopback(const std::vector<bool> &inputs, std::vector<bool> &outputs)
//{
//    //Initialize blocks
//    Mapper * mapper             = new Mapper(1, Mapper::CONST_SET_BPSK);
//    Demapper * demapper         = new Demapper(Mapper::CONST_SET_BPSK, true);
//
//    //Test symbol mapping
//    FilterChain testChain = *demapper + *mapper;
//    testChain.init();
//    (void)runFilters(testChain, inputs, outputs);
//    checkOutputs(inputs, outputs, 0);
//}
//
//static void symbolMappingFFTLoopback(const std::vector<bool> &inputs, std::vector<bool> &outputs)
//{
//    //Initialize blocks
//    Mapper * mapper             = new Mapper(1, Mapper::CONST_SET_BPSK);
//    FFT * ifft                  = new FFT(1024, true);
//    ifft->setOutputFormat(FFT_OUTPUT_WL, -2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
//
//    Demapper * demapper         = new Demapper(Mapper::CONST_SET_BPSK, true);
//    FFT * fft                   = new FFT(1024, false);
//    fft->setOutputFormat(FFT_OUTPUT_WL, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
//
//
//    //Test symbol mapping + FFT
//    FilterChain testChain = *demapper + *fft + *ifft + *mapper;
//    size_t numOutputs = runFilters(testChain, inputs, outputs);
//    std::cout << "FFT Loopback has " << numOutputs << " outputs" << std::endl;
//    checkOutputs(inputs, outputs, 0);
//}

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
    Mapper * mapper             = new Mapper(UPSAMPLE_FACTOR, Mapper::CONST_SET_BPSK);
    FFT * ifft                  = new FFT(FFT_SIZE, true, UPSAMPLE_FACTOR);
    ifft->setOutputFormat(FFT_OUTPUT_WL, -2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    DigitalUpConverter * duc    = new DigitalUpConverter(-MIXER_FREQ, up2Coeffs, up5Coeffs);


    Demapper * demapper         = new Demapper(Mapper::CONST_SET_BPSK, true);
    FFT * fft                   = new FFT(FFT_SIZE, false, UPSAMPLE_FACTOR);
    fft->setOutputFormat(FFT_OUTPUT_WL, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);
    DigitalDownConverter *ddc   = new DigitalDownConverter(MIXER_FREQ, down2Coeffs, down5Coeffs);

    //Debug probes
    std::string duc_in_probe_name = "DUC_INPUT";
    std::string ddc_out_probe_name = "DDC_OUTPUT";
    SampleCountTrigger *duc_in   = new SampleCountTrigger(duc_in_probe_name, FilterProbe::CSV, FFT_SIZE*2, 1, 0);
    SampleCountTrigger *ddc_out  = new SampleCountTrigger(ddc_out_probe_name, FilterProbe::CSV, FFT_SIZE*2, 1, 0);


    //Test symbol mapping + FFT + DUC
//    FilterChain testChain =  *demapper + *fft + *ddc_out + *ddc + *duc + *duc_in + *ifft + *mapper;
    FilterChain testChain =  *demapper + *ddc_out + *ddc + *duc + *duc_in + *mapper;

    constexpr size_t INPUT_UPSAMPLE_FACTOR = 1;
    size_t numOutputs = runFilters(testChain, inputs, outputs, INPUT_UPSAMPLE_FACTOR);
    std::cout << "FFT/DUC Loopback has " << numOutputs << " outputs" << std::endl;

    constexpr size_t TIME_DOMAIN_DELAY = 64; //delay from DDC/DUC filtering
//    checkOutputs(inputs, outputs, TIME_DOMAIN_DELAY + FFT_SIZE, INPUT_UPSAMPLE_FACTOR);
    checkOutputs(inputs, outputs, TIME_DOMAIN_DELAY, INPUT_UPSAMPLE_FACTOR);
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

//    Logger::info("Symbol Mapping/FFT Loopback Starting...");
//    symbolMappingFFTLoopback(inputs, outputs);
//    Logger::info("Symbol Mapping/FFT Loopback Finished.");

    symbolMappingFFTDUCLoopback(down2, down5, up2, up5, inputs, outputs);

//    Logger::info("Symbol Mapping Loopback Starting...");
//    symbolMappingLoopback(inputs, outputs);
//    Logger::info("Symbol Mapping Loopback Finished.");

    return 0;
}
