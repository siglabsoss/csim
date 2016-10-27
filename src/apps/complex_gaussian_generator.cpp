#include <mathlib/complex_gaussian_noise.hpp>
#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[])
{
    int c;
    double variance = 0;
    int count = 0;
    int gotVariance = 0;
    int gotCount = 0;

    while ((c = getopt(argc, argv, "v:n:")) != -1) {
        switch(c) {
        case 'v':
            variance = atof(optarg);
            gotVariance = 1;
            break;
        case 'n':
            count = atoi(optarg);
            gotCount = 1;
            break;
        }
    }

    if (gotVariance == 0 || gotCount == 0 || count <= 0) {
        std::cout << "Usage: " << argv[0] << " -v <variance> -n <count>" << std::endl;
        return 1;
    }

    ComplexGaussianNoise noise(variance);

    std::cerr << "Generating " << count << " samples with variance of " << variance << std::endl;

    for (size_t i = 0; i < static_cast<size_t>(count); i++) {
        ComplexDouble val = noise.getNext();
        std::cout << val.real() << "," << val.imag() << std::endl;
    }
    return 0;
}
