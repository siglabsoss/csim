#include <test/unit_test.hpp>

#define private public
#include <filters/biquad.hpp>
#undef private

#include <utils/utils.hpp>

CSIM_TEST_SUITE_BEGIN(BiquadVerification)

static void runFilter(const std::string &inputFile, const std::string &outputFile, double avgErrThreshold, double b0, double b1, double b2, double a1, double a2)
{
    Biquad bi;
    bi.init(b0, b1, b2, a1, a2);
    std::vector<FixedComplexNorm16> inputs = complexRead16Unscaled(inputFile);
    std::vector<FixedComplexNorm16> outputs = complexRead16Unscaled(outputFile);

    BOOST_CHECK_EQUAL(inputs.size(), outputs.size());
    BOOST_CHECK(inputs.size() > 0);

    filter_io_t input, output;
    double realErrAccum = 0.0;
    double imagErrAccum = 0.0;
    for (size_t i = 0; i < inputs.size(); i++) {
        input = inputs[i];
        bi.input(input);
        bi.tick();
        BOOST_CHECK(bi.output(output) == true);

        //std::cout << output.fc.real() << "," << output.fc.imag() << std::endl;

        double expectedReal = outputs[i].real().to_double();
        double actualReal = output.fc.real().to_double();
        double expectedImag = outputs[i].imag().to_double();
        double actualImag = output.fc.imag().to_double();

        double realError = 0.0;
        double imagError = 0.0;

        if (actualReal > 0.0) {
            realError = abs((expectedReal - actualReal) / actualReal);
        }
        if (actualImag > 0.0) {
            imagError = abs((expectedImag - actualImag) / actualImag);
        }


        realErrAccum += realError;
        imagErrAccum += imagError;

//        std::cout << output << std::endl;
//        if (i > 3) break;

        //We only want to tolerate a certain error threshold, but we excuse really small values as they have a tendency to have larger errors
        if (expectedReal > 0.002) {
            BOOST_CHECK_MESSAGE(realError < 0.01, "Sample #" << i << " filter output (real) = " << actualReal << " Expected output (real) = " << expectedReal << " (error = " << realError << ")");
        } else {
            BOOST_CHECK_MESSAGE(realError < 0.03, "Sample #" << i << " filter output (real) = " << actualReal << " Expected output (real) = " << expectedReal << " (error = " << realError << ")");
        }
        if (expectedImag > 0.002) {
            BOOST_CHECK_MESSAGE(imagError < 0.01, "Sample #" << i << " filter output (imag) = " << actualImag << " Expected output (imag) = " << expectedImag << " (error = " << imagError << ")");
        } else {
            BOOST_CHECK_MESSAGE(imagError < 0.03, "Sample #" << i << " filter output (imag) = " << actualImag << " Expected output (imag) = " << expectedImag << " (error = " << imagError << ")");
        }
        //std::cout << input << std::endl;
    }

    double avgRealErr = realErrAccum / inputs.size();
    double avgImagErr = imagErrAccum / inputs.size();
    BOOST_CHECK_MESSAGE(avgRealErr < avgErrThreshold, "Average error (real) = " << avgRealErr);
    BOOST_CHECK_MESSAGE(avgImagErr < avgErrThreshold, "Average error (imag) = " << avgImagErr);
    std::cout << "Average Error (real) = " << avgRealErr << std::endl;
    std::cout << "Average Error (imag) = " << avgImagErr << std::endl;
}


CSIM_TEST_CASE(FIXED_POINT_SINE_WAVE_INPUT)
{
    runFilter("./data/biquad/input/biquad_input1.csv", "./data/biquad/output/biquad_output1.csv", 0.0001, 1, 0.3, 0.4, -0.1, -0.2);
}


CSIM_TEST_CASE(FLOATING_POINT_CHIRP_INPUT)
{
    runFilter("./data/biquad/input/biquad_input2.csv", "./data/biquad/output/biquad_output2.csv", 0.0002, 1, 0.3, 0.4, -0.1, -0.2);
}
/*
CSIM_TEST_CASE(FLOATING_POINT_CHIRP_LOWPASS_COEFFS)
{
    runFilter("./data/biquad/input/biquad_input3.csv", "./data/biquad/output/biquad_output3.csv", 0.00002, 0.01069769807630125, 0.0213953961526025, 0.01069769807630125, -0.8314990838150726, -0.12571012387972225);
}
*/

CSIM_TEST_SUITE_END()
