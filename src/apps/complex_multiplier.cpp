#include <utils/utils.hpp>
#include <types/fixedcomplex.hpp>

void runSamples(std::vector<std::pair<ComplexDouble, ComplexDouble> >& inputs)
{
    for (size_t i = 0; i < inputs.size(); ++i) {
        SLFixComplex a(5,
                       1,
                       SLFixPoint::QUANT_RND_HALF_UP,
                       SLFixPoint::OVERFLOW_SATURATE);
        SLFixComplex b(5,
                       1,
                       SLFixPoint::QUANT_RND_HALF_UP,
                       SLFixPoint::OVERFLOW_SATURATE);
        SLFixPoint val(5,
                       1,
                       SLFixPoint::QUANT_RND_HALF_UP,
                       SLFixPoint::OVERFLOW_SATURATE);

        SLFixComplex c(7,
                       2,
                       SLFixPoint::QUANT_RND_HALF_UP,
                       SLFixPoint::OVERFLOW_SATURATE);

        // The input file is expected to have raw integer values
        // val = static_cast<int64_t>(inputs[i].first.real());
        // a.real(val);
        // val = static_cast<int64_t>(inputs[i].first.imag());
        // a.imag(val);
        //
        // val = static_cast<int64_t>(inputs[i].second.real());
        // b.real(val);
        // val = static_cast<int64_t>(inputs[i].second.imag());
        // b.imag(val);

        a.real(inputs[i].first.real() / 16.0);
        a.imag(inputs[i].first.imag() / 16.0);

        b.real(inputs[i].second.real() / 16.0);
        b.imag(inputs[i].second.imag() / 16.0);

        c = a * b;

        std::cout << a.real().to_int64() << "," << a.imag().to_int64() <<
        "," << b.real().to_int64() << "," << b.imag().to_int64() << "," <<
        c.real().to_int64() << "," << c.imag().to_int64() << std::endl;

        // std::cout << inputs[i].first << "," << inputs[i].second << std::endl;
        // std::cout << a.real().to_int64() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    bool gotInputFile = false;
    std::string inputFile;
    int c;

    while ((c = getopt(argc, argv, "f:i:w:sx")) != -1) {
        switch (c) {
        case 'f':
            inputFile    = std::string(optarg);
            gotInputFile = true;
            break;
        }
    }

    if (gotInputFile != true) {
        std::cout << "Usage: " << argv[0] << " -f <input filename> " << std::endl;
        return 1;
    }

    std::vector<std::pair<ComplexDouble,
                          ComplexDouble> > inputs =
        utils::readComplexPairFromCSV<ComplexDouble>(
            inputFile);

    assert(inputs.size() > 0);

    runSamples(inputs);
}
