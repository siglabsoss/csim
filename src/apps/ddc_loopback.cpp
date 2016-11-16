#include <filters/ddc_duc/ddc_utils.hpp>
#include <utils/utils.hpp>
#include <iostream>

//static double angleDiff(double a, double b)
//{
//    double result = std::abs(a - b);
//    if (result > M_PI) {
//        return (2 * M_PI) - result;
//    }
//    return result;
//}

int main(int argc, char *argv[])
{
    if (argc != 6) {
        std::cout << "Usage: " << argv[0] << " <ddc_down2_coeffs> <ddc_down5_coeffs> <duc_up2_coeffs> <duc_up5_coeffs> <test_inputs>" << std::endl;
        return 1;
    }

    std::string down2(argv[1]);
    std::string down5(argv[2]);
    std::string up2(argv[3]);
    std::string up5(argv[4]);
    std::string inputFile(argv[5]);

    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(inputFile);
    std::vector<ComplexDouble> outputs;
    DDCUtils::runLoopbackTest(down2, down5, up2, up5, inputs, outputs);

    for (size_t i = 0; i < outputs.size(); i++) {
        std:: cout << inputs[i].real() << "," << inputs[i].imag() << "," << outputs[i].real() << "," << outputs[i].imag() << std::endl;
    }

    return 0;
}
