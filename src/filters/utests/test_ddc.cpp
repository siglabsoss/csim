#include <test/unit_test.hpp>

#include <filters/ddc.hpp>
#include <utils/utils.hpp>

#include <fstream>
#include <cmath>

CSIM_TEST_SUITE_BEGIN(DigitalDownConverterVerification)

// Number of iterations to run simulation
static constexpr int MAX_ITERS  = 200000;
static constexpr int PWIDTH     = 32;
static constexpr int INWIDTH    = 16;
static constexpr int OUTWIDTH   = 18;

static void runFilter(const std::string &halfbandCoeffFile, const std::string &by5CoeffFile)
{
    SLFixPoint::throwOnOverflow = true;
    std::vector<ComplexDouble> halfbandComplexCoeffs = readComplexFromCSV<ComplexDouble>(halfbandCoeffFile);
    std::vector<ComplexDouble> by5ComplexCoeffs      = readComplexFromCSV<ComplexDouble>(by5CoeffFile);

    assert(halfbandComplexCoeffs.size() != 0 && by5ComplexCoeffs.size() != 0);

    std::vector<double> halfbandCoeffs(halfbandComplexCoeffs.size());
    std::vector<double> by5Coeffs(by5ComplexCoeffs.size());

    for (size_t i = 0; i < halfbandComplexCoeffs.size(); i++) {
        halfbandCoeffs[i] = halfbandComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < by5ComplexCoeffs.size(); i++) {
        by5Coeffs[i] = by5ComplexCoeffs[i].real();
    }

    double frequency = 0.16;

    SLFixedPoint<INWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> input_sample;
    SLFixedPoint<PWIDTH, 0, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> phase_increment;
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> inph;
    SLFixedPoint<OUTWIDTH, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE> quad;

    DigitalDownConverter ddc(frequency, halfbandCoeffs, by5Coeffs);
    filter_io_t data;

    for (int ii = 0; ii < MAX_ITERS; ++ii) {
        input_sample = 0.1
            + 0.125 * std::cos(2*M_PI*(frequency + 0.005)*ii + 0.72)
            + 0.125 * std::cos(2*M_PI*(0.5 * frequency)*ii)
            + 0.125 * std::cos(2*M_PI*(frequency - 0.005)*ii + 0.72)
            + 0.125 * std::cos(2*M_PI*(frequency + 0.01)*ii - 1.333)
            + 0.125 * std::cos(2*M_PI*(0.25 * frequency)*ii)
            + 0.125 * std::cos(2*M_PI*(frequency - 0.01)*ii - 1.333);

        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(input_sample.wl(), input_sample.iwl());
        data.fc.real(input_sample);
        data.fc.imag(0.0);
        ddc.input(data);
        if (ddc.output(data)) {
            //std::cout << data.fc.real().to_double() << "," << data.fc.imag().to_double() << std::endl;
        }
    }
}

CSIM_TEST_CASE(BASIC_FUNCTIONALITY)
{
    runFilter("./data/ddc/coeffs/halfband.txt", "./data/ddc/coeffs/downby5.txt");
}

CSIM_TEST_SUITE_END()
