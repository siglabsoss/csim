#include <core/filter_chain.hpp>
#include <core/logger.hpp>

#include <filters/fft.hpp>
#include <filters/demapper.hpp>
#include <filters/mapper.hpp>

static constexpr size_t NUM_INPUT_SAMPLES = 1024 * 4;

static void loadInput(std::vector<bool> &inputs)
{
    for (size_t i = 0; i < NUM_INPUT_SAMPLES; i++) {
        bool bit = static_cast<bool>(std::rand() & 1);
        inputs[i] = bit;
    }
}

static void checkOutputs(const std::vector<bool> &inputs, const std::vector<bool> &outputs, size_t numOutputs)
{
    for (size_t i = 0; i < numOutputs; i++) {
        if (inputs[i] != outputs[i]) {
            std::cout << "#" << i << " " << inputs[i] << " != " << outputs[i] << std::endl;
        }
    }
}

static size_t runFilters(FilterChain &chain, const std::vector<bool> &inputs, std::vector<bool> &outputs)
{
    filter_io_t sample;
    size_t numOutputs = 0;
    for (size_t i = 0; i < inputs.size(); i++) {
        sample.type = IO_TYPE_BIT;
        sample.bit = inputs[i];
        assert(chain.input(sample));
        chain.tick();
        if (chain.output(sample)) {
            assert(sample.type == IO_TYPE_BIT);
            outputs[numOutputs] = sample.bit;
//            std::cout << "output #" << numOutputs << std::endl;
            numOutputs++;
        }
    }
    return numOutputs;
}

static void symbolMappingLoopback(const std::vector<bool> &inputs, std::vector<bool> &outputs)
{
    //Initialize blocks
    Mapper * mapper             = new Mapper(1, Mapper::CONST_SET_BPSK);
    Demapper * demapper         = new Demapper(Mapper::CONST_SET_BPSK, true);

    //Test symbol mapping
    FilterChain testChain = *demapper + *mapper;
    testChain.init();
    size_t numOutputs = runFilters(testChain, inputs, outputs);
    checkOutputs(inputs, outputs, numOutputs);
}

static void symbolMappingFFTLoopback(const std::vector<bool> &inputs, std::vector<bool> &outputs)
{
    //Initialize blocks
    Mapper * mapper             = new Mapper(1, Mapper::CONST_SET_BPSK);
    FFT * ifft                  = new FFT(1024, true);
    ifft->setOutputFormat(FFT_OUTPUT_WL, -2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);

    Demapper * demapper         = new Demapper(Mapper::CONST_SET_BPSK, true);
    FFT * fft                   = new FFT(1024, false);
    fft->setOutputFormat(FFT_OUTPUT_WL, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);


    //Test symbol mapping + FFT
    FilterChain testChain = *demapper + *fft + *ifft + *mapper;
    size_t numOutputs = runFilters(testChain, inputs, outputs);
    std::cout << "FFT Loopback has " << numOutputs << " outputs" << std::endl;
    checkOutputs(inputs, outputs, numOutputs);
}

int main(int argc, char *argv[])
{
    std::srand(1473294057+1);

    std::vector<bool> inputs(NUM_INPUT_SAMPLES);
    std::vector<bool> outputs(NUM_INPUT_SAMPLES);


    //Calculate random inputs
    loadInput(inputs);

    Logger::info("Symbol Mapping Loopback Starting...");
    symbolMappingLoopback(inputs, outputs);
    Logger::info("Symbol Mapping Loopback Finished.");

    Logger::info("Symbol Mapping/FFT Loopback Starting...");
    symbolMappingFFTLoopback(inputs, outputs);
    Logger::info("Symbol Mapping/FFT Loopback Finished.");

    return 0;
}
