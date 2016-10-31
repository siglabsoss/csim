#include <filters/fft.hpp>
#include <utils/utils.hpp>
#include <types/fixedcomplex.hpp>

static constexpr size_t N_FFT_POINTS = 1024;

int main(int argc, char *argv[])
{
    int c;
    bool gotInputFile = false, shouldScale = false;
    std::string inputFile;
    while ((c = getopt(argc, argv, "f:s")) != -1) {
        switch(c) {
        case 'f':
            inputFile = std::string(optarg);
            gotInputFile = true;
            break;
        case 's':
            shouldScale = true;
        }
    }

    if (gotInputFile != true) {
        std::cout << "Usage: " << argv[0] << " -f <input filename>" << "[-s (scaled output)]" << std::endl;
        return 1;
    }

    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(inputFile);
    assert(inputs.size() == N_FFT_POINTS);
    std::vector<SLFixComplex> outputs(inputs.size());
    std::vector<SLFixComplex> inputsFP(inputs.size());

    FFT fft(N_FFT_POINTS, false);
    filter_io_t data;
    size_t outputCount = 0;
    size_t inWl = 0;
    size_t inIwl = 0;
    //First iteration primes the pump, second iteration pushes the outputs out
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < N_FFT_POINTS; j++) {
            data.type = IO_TYPE_COMPLEX_FIXPOINT;
            inputsFP[j].setFormat(FFT_INPUT_FORMAT);
            inputsFP[j].set(inputs[j].real(), inputs[j].imag());
            data.fc.setFormat(inputsFP[j]);
            data.fc = inputsFP[j];
            inWl = data.fc.wl();
            inIwl = data.fc.iwl();

            fft.input(data);
            fft.tick();
            bool didGetOutput = fft.output(data);
            bool lastInput = (i == 1 && j == N_FFT_POINTS - 1);
            if (didGetOutput && !lastInput) {
                outputs[outputCount].setFormat(data.fc);
                outputs[outputCount] = data.fc;
                ++outputCount;
            }//If output is ready
        }
    }

    std::cerr << "Running " << N_FFT_POINTS << " FFT with inputs from file." << inputFile << std::endl;
    std::cerr << "Input format is Q" << inIwl << "." << (inWl - inIwl) << " Output format is Q" << outputs[0].iwl() << "." << outputs[0].wl() - outputs[0].iwl() << std::endl;

    std::cout << "InReal,InImag,OutReal,OutImag" << std::endl;
    for (size_t j = 0; j < N_FFT_POINTS; j++) {
        if (shouldScale == false) {
            std::cout << inputsFP[j].real().to_double() << "," << inputsFP[j].imag().to_double() << "," << outputs[j].real().to_double() << "," << outputs[j].imag().to_double() << std::endl;
        } else {
            std::cout << inputsFP[j].real().to_int64() << "," << inputsFP[j].imag().to_int64() << "," << outputs[j].real().to_int64() << "," << outputs[j].imag().to_int64() << std::endl;
        }
    }

    return 0;
}
