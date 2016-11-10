#include <test/unit_test.hpp>

#include <filters/ddc.hpp>
#include <filters/duc.hpp>
#include <utils/utils.hpp>

#include <fstream>
#include <cmath>

CSIM_TEST_SUITE_BEGIN(DigitalDownConverterVerification)

// Number of iterations to run simulation
static constexpr int MAX_ITERS  = 2000000; //1068576;
static constexpr double MIXER_FREQ = 0.16;

static void runDDC(const std::string &halfbandCoeffFile, const std::string &by5CoeffFile, const std::string &answersFile)
{
    SLFixPoint::throwOnOverflow = true;
    std::vector<ComplexDouble> halfbandComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>(halfbandCoeffFile);
    std::vector<ComplexDouble> by5ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(by5CoeffFile);
    std::vector<ComplexDouble> answers               = utils::readComplexFromCSV<ComplexDouble>(answersFile);

    assert(answers.size() > 0);

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

    size_t outputCount = 0;
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
            double realDiff = fabs(data.fc.real().to_double() - answers[outputCount].real());
            double imagDiff = fabs(data.fc.imag().to_double() - answers[outputCount].imag());
            ++outputCount;
            BOOST_CHECK(realDiff < 1.0 / (1ull << DDC_INPUT_WL));
            BOOST_CHECK(imagDiff < 1.0 / (1ull << DDC_INPUT_WL));
            //std::cout << data.fc.real().to_double() << "," << data.fc.imag().to_double() << std::endl;
        }
    }
}

static void runDUC(const std::string &up2CoeffFile, const std::string &up5CoeffFile, const std::string &inputsFile, const std::string &answersFile)
{
    SLFixPoint::throwOnOverflow = true;
    std::vector<ComplexDouble> up2ComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>(up2CoeffFile);
    std::vector<ComplexDouble> up5ComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>(up5CoeffFile);

    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(inputsFile);
    std::vector<ComplexDouble> answers = utils::readComplexFromCSV<ComplexDouble>(answersFile);

    assert(answers.size() > 0);

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

    size_t outputCount = 0;

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
//                double realDiff = fabs(data.fc.real().to_double() - answers[outputCount].real());
//                double imagDiff = fabs(data.fc.imag().to_double() - answers[outputCount].imag());
//                BOOST_CHECK(realDiff < 1.0 / (1ull << DUC_INPUT_WL));
//                BOOST_CHECK(imagDiff < 1.0 / (1ull << DUC_INPUT_WL));
                //std::cout << data.fc.real().to_double() << "," << data.fc.imag().to_double() << std::endl;
                outputCount++;
            }
        }
    }
}

static void runLoopbackTest(const std::string &halfbandCoeffFile, const std::string &by5CoeffFile, const std::string &up2CoeffFile, const std::string &up5CoeffFile)
{
    std::vector<ComplexDouble> halfbandComplexCoeffs = utils::readComplexFromCSV<ComplexDouble>(halfbandCoeffFile);
    std::vector<ComplexDouble> by5ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(by5CoeffFile);
    std::vector<ComplexDouble> up2ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(up2CoeffFile);
    std::vector<ComplexDouble> up5ComplexCoeffs      = utils::readComplexFromCSV<ComplexDouble>(up5CoeffFile);
    assert(halfbandComplexCoeffs.size() != 0 && by5ComplexCoeffs.size() != 0);
    assert(up2CoeffFile.size() != 0 && up5CoeffFile.size() != 0);

    std::vector<double> halfbandCoeffs(halfbandComplexCoeffs.size());
    std::vector<double> by5Coeffs(by5ComplexCoeffs.size());
    std::vector<double> up2Coeffs(up2ComplexCoeffs.size());
    std::vector<double> up5Coeffs(up5ComplexCoeffs.size());

    for (size_t i = 0; i < halfbandComplexCoeffs.size(); i++) {
        halfbandCoeffs[i] = halfbandComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < by5ComplexCoeffs.size(); i++) {
        by5Coeffs[i] = by5ComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < up2ComplexCoeffs.size(); i++) {
        up2Coeffs[i] = up2ComplexCoeffs[i].real();
    }

    for (size_t i = 0; i < up5ComplexCoeffs.size(); i++) {
        up5Coeffs[i] = up5ComplexCoeffs[i].real();
    }

    DigitalDownConverter ddc(MIXER_FREQ, halfbandCoeffs, by5Coeffs);
    DigitalUpConverter   duc(-MIXER_FREQ, up2Coeffs, up5Coeffs);

    size_t numInputs = 1000;
    SLFixedPoint<DUC_INPUT_FP_FORMAT> inph_in;
    SLFixedPoint<DUC_INPUT_FP_FORMAT> quad_in;
    filter_io_t input, output;
    size_t outputCount = 0;
    for (size_t ii = 0; ii < numInputs; ii++) {
        inph_in = (ii & 1) ? 1.0 : -1.0;
        quad_in = 0.0;

        input.type = IO_TYPE_COMPLEX_FIXPOINT;
        input.fc.setFormat(inph_in.wl(), inph_in.iwl());
        input.fc.real(inph_in);
        input.fc.imag(quad_in);

//        std::cout << "in " << outputCount << ": " << input << std::endl;
        duc.input(input);

        for (size_t i = 0; i < 10; i++) {
            duc.tick();
            if (duc.output(output)) {
//                std::cout << "duc: " << data << std::endl;
                ddc.input(output);
            }
            ddc.tick();
            if (ddc.output(output)) {
                outputCount++;
                //XXX compare magnitude/phase
                if (outputCount > 100) {
                    BOOST_CHECK(std::abs(input.fc.real().to_double() - output.fc.real().to_double()) < 0.1);
                    BOOST_CHECK(std::abs(input.fc.imag().to_double() - output.fc.imag().to_double()) < 0.1);
                }
            }
        }
    }
}

CSIM_TEST_CASE(BASIC_DDC_FUNCTIONALITY)
{
    //runDDC("./data/ddc/coeffs/halfband.txt", "./data/ddc/coeffs/downby5.txt", "./data/ddc/ddc_out.csv");
}

CSIM_TEST_CASE(BASIC_DUC_FUNCTIONALITY)
{
    //runDUC("./data/ddc/coeffs/halfband_interp.txt", "./data/ddc/coeffs/by5_interp.txt", "./data/ddc/ddc_out.csv", "./data/ddc/duc_out.csv");
}

CSIM_TEST_CASE(LOOPBACK_TEST)
{
    runLoopbackTest("./data/ddc/coeffs/halfband.txt", "./data/ddc/coeffs/downby5.txt", "./data/ddc/coeffs/halfband_interp.txt", "./data/ddc/coeffs/by5_interp.txt");
}

CSIM_TEST_SUITE_END()
