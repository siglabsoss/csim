#include <utils/utils.hpp>

static void runFilter(const std::vector<ComplexDouble>& inputs)
{
        printf("Running filter with %ld input samples\n", inputs.size());

    for (unsigned i = 0; i < inputs.size(); ++i) {
        printf("%f,%f\n",
               inputs[i].real(), inputs[i].imag());
    }

    // Write filter here
}

int main(int argc, char *argv[])
{
    int  c;
    bool gotInputFile = false;

    std::string inputFile;

    while ((c = getopt(argc, argv, "f:")) != -1) {
        switch (c) {
        case 'f':
            inputFile    = std::string(optarg);
            gotInputFile = true;
            break;
        }
    }

    if (gotInputFile != true) {
        std::cout << "Usage: " << argv[0] << " -f <input filename>" << std::endl;
        return 1;
    }

    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(
        inputFile);

    assert(inputs.size() > 0);

    runFilter(inputs);

    return 0;
}
