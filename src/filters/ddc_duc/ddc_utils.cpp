#include <filters/ddc_duc/ddc_utils.hpp>
#include <filters/ddc_duc/ddc.hpp>
#include <filters/ddc_duc/duc.hpp>
#include <utils/utils.hpp>

static constexpr double MIXER_FREQ = 0.16;
void DDCUtils::runLoopbackTest(const std::string &halfbandCoeffFile, const std::string &by5CoeffFile,
        const std::string &up2CoeffFile, const std::string &up5CoeffFile,
        const std::vector<ComplexDouble> &inputs, std::vector<ComplexDouble> &outputs)
{
    // Prepare coefficients
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

    assert(inputs.size() > 0);
    assert(outputs.size() == 0);

    DigitalDownConverter ddc(MIXER_FREQ, halfbandCoeffs, by5Coeffs);
    DigitalUpConverter   duc(-MIXER_FREQ, up2Coeffs, up5Coeffs);
    filter_io_t input, output;

    for (size_t i = 0; i < inputs.size(); i++) {
        input.type = IO_TYPE_COMPLEX_FIXPOINT;
        input.fc.setFormat(DUC_INPUT_FP_FORMAT);
        input.fc.real(inputs[i].real());
        input.fc.imag(inputs[i].imag());

        duc.input(input);
        for (size_t i = 0; i < 10; i++) {
            duc.tick();
            if (duc.output(output)) {
//                std::cout << "duc: " << data << std::endl;
                ddc.input(output);
            }
            ddc.tick();
            if (ddc.output(output)) {
                outputs.push_back(output.toComplexDouble());
            }
        }
    }
}
