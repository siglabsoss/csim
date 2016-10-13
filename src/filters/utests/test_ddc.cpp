#include <test/unit_test.hpp>

#include <filters/ddc.hpp>
#include <filters/duc.hpp>
#include <utils/utils.hpp>

#include <fstream>
#include <cmath>

CSIM_TEST_SUITE_BEGIN(DigitalDownConverterVerification)

// Number of iterations to run simulation
static constexpr int MAX_ITERS  = 200000; //1068576;
static constexpr double MIXER_FREQ = 0.16;

static void runDDC(const std::string &halfbandCoeffFile, const std::string &by5CoeffFile)
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

    SLFixedPoint<DDC_INPUT_FP_FORMAT> input_sample;

    DigitalDownConverter ddc(MIXER_FREQ, halfbandCoeffs, by5Coeffs);
    filter_io_t data;

    for (int ii = 0; ii < MAX_ITERS; ++ii) {
        input_sample = 0.1
            + 0.125 * std::cos(2*M_PI*(MIXER_FREQ + 0.005)*ii + 0.72)
            + 0.125 * std::cos(2*M_PI*(0.5 * MIXER_FREQ)*ii)
            + 0.125 * std::cos(2*M_PI*(MIXER_FREQ - 0.005)*ii + 0.72)
            + 0.125 * std::cos(2*M_PI*(MIXER_FREQ + 0.01)*ii - 1.333)
            + 0.125 * std::cos(2*M_PI*(0.25 * MIXER_FREQ)*ii)
            + 0.125 * std::cos(2*M_PI*(MIXER_FREQ - 0.01)*ii - 1.333);

        //input_sample = 0.5 * std::cos(2*M_PI*(MIXER_FREQ)*ii + 0.72);

        //std::cout << input_sample << ",0" << std::endl;

        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(input_sample.wl(), input_sample.iwl());
        data.fc.real(input_sample);
        data.fc.imag(0.0);
        ddc.input(data);
        if (ddc.output(data)) {
            std::cout << data.fc.real().to_double() << "," << data.fc.imag().to_double() << std::endl;
        }
    }
}

static void runDUC(const std::string &up2CoeffFile, const std::string &up5CoeffFile, const std::string &inputsFile)
{
    SLFixPoint::throwOnOverflow = true;
    std::vector<ComplexDouble> up2ComplexCoeffs = readComplexFromCSV<ComplexDouble>(up2CoeffFile);
    std::vector<ComplexDouble> up5ComplexCoeffs = readComplexFromCSV<ComplexDouble>(up5CoeffFile);

    std::vector<ComplexDouble> inputs = readComplexFromCSV<ComplexDouble>(inputsFile);

    assert(inputs.size() != 0);
    assert(up2CoeffFile.size() != 0 && up5CoeffFile.size() != 0);

    std::vector<double> up2Coeffs(up2ComplexCoeffs.size());
    std::vector<double> up5Coeffs(up5ComplexCoeffs.size());

    for (size_t i = 0; i < up2ComplexCoeffs.size(); i++) {
        up2Coeffs[i] = up2ComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < up5ComplexCoeffs.size(); i++) {
        up5Coeffs[i] = up5ComplexCoeffs[i].real();
    }

    SLFixedPoint<DUC_INPUT_FP_FORMAT> inph_in;
    SLFixedPoint<DUC_INPUT_FP_FORMAT> quad_in;

    DigitalUpConverter duc(-MIXER_FREQ, up2Coeffs, up5Coeffs);
    filter_io_t data;

    for (size_t ii = 0; ii < inputs.size(); ii++) {
        inph_in = inputs[ii].real();
        quad_in = inputs[ii].imag();

        data.type = IO_TYPE_COMPLEX_FIXPOINT;
        data.fc.setFormat(inph_in.wl(), inph_in.iwl());
        data.fc.real(inph_in);
        data.fc.imag(quad_in);

        duc.input(data);
        for (size_t i = 0; i < 10; i++) {
            duc.tick();
            if (duc.output(data)) {
                std::cout << data.fc.real().to_double() << "," << data.fc.imag().to_double() << std::endl;
            }
        }
    }
}

//CSIM_TEST_CASE(BASIC_DDC_FUNCTIONALITY)
//{
//    runDDC("./data/ddc/coeffs/halfband.txt", "./data/ddc/coeffs/downby5.txt");
//}

//CSIM_TEST_CASE(BASIC_DUC_FUNCTIONALITY)
//{
//    runDUC("./data/ddc/coeffs/halfband.txt", "./data/ddc/coeffs/downby5.txt", "./data/ddc/ddc_out_new.txt");
//}

CSIM_TEST_SUITE_END()
