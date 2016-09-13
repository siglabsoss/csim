#include <test/unit_test.hpp>

#define private public
#include <filters/biquad_iir.hpp>
#undef private

#include <utils/utils.hpp>

CSIM_TEST_SUITE_BEGIN(BiquadVerification)

static void runFilter(const std::string &inputFile, const std::string &outputFile, size_t coeffBitWidth, double avgErrThreshold, const std::vector<Biquad::SOSCoeffs> &coeffs)
{
    BiquadIIR bi(coeffs.size());
    //Biquad bi(coeffBitWidth);
    bi.init(coeffs);
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

        double realErrorAbs = 0.0;
        double imagErrorAbs = 0.0;

        realErrorAbs = abs((expectedReal - actualReal));
        imagErrorAbs = abs((expectedImag - actualImag));

        realErrAccum += realErrorAbs;
        imagErrAccum += imagErrorAbs;

        double absErrorThreshold = 3.0517578125e-05; //for Q1.15 one bit is (1 / 2^15) thus staying under this threshold means that our error is from quantization only (unavoidable)
        BOOST_CHECK_MESSAGE(realErrorAbs <= absErrorThreshold, "Sample #" << i << " filter output (real) = " << actualReal << " Expected output (real) = " << expectedReal << " (error = " << realErrorAbs << ")");

        BOOST_CHECK_MESSAGE(imagErrorAbs <= absErrorThreshold, "Sample #" << i << " filter output (imag) = " << actualImag << " Expected output (imag) = " << expectedImag << " (error = " << imagErrorAbs << ")");

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
    Biquad::SOSCoeffs coeffs = {
            .b0 = 1,
            .b1 = 0.3,
            .b2 = 0.4,
            .a1 = -0.1,
            .a2 = -0.2
    };
    std::vector<Biquad::SOSCoeffs> coeffList(1);
    coeffList[0] = coeffs;
    runFilter("./data/biquad/input/biquad_input1.csv", "./data/biquad/output/biquad_output1.csv", 16, 0.0001, coeffList);
}


CSIM_TEST_CASE(FLOATING_POINT_CHIRP_INPUT)
{
    Biquad::SOSCoeffs coeffs = {
            .b0 = 1,
            .b1 = 0.3,
            .b2 = 0.4,
            .a1 = -0.1,
            .a2 = -0.2
    };
    std::vector<Biquad::SOSCoeffs> coeffList(1);
    coeffList[0] = coeffs;
    runFilter("./data/biquad/input/biquad_input2.csv", "./data/biquad/output/biquad_output2.csv", 16, 0.0002, coeffList);
}

CSIM_TEST_CASE(FLOATING_POINT_CHIRP_LOWPASS_COEFFS)
{
    Biquad::SOSCoeffs coeffs = {
            .b0 = 0.01069769807630125,
            .b1 = 0.0213953961526025,
            .b2 = 0.01069769807630125,
            .a1 = -0.8314990838150726,
            .a2 = -0.12571012387972225
    };
    std::vector<Biquad::SOSCoeffs> coeffList(1);
    coeffList[0] = coeffs;
    runFilter("./data/biquad/input/biquad_input3.csv", "./data/biquad/output/biquad_output3.csv", 16, 0.00002, coeffList);
}


CSIM_TEST_CASE(FLOATING_POINT_CHIRP_BANDPASS)
{
    Biquad::SOSCoeffs coeffs = {
            .b0 = 0.354736571634042,
            .b1 = 0.0,
            .b2 = -0.354736571634042,
            .a1 = 0.0924708822220468,
            .a2 = -0.290526856731916
    };
    std::vector<Biquad::SOSCoeffs> coeffList(1);
    coeffList[0] = coeffs;
    runFilter("./data/biquad/input/biquad_input4.csv", "./data/biquad/output/biquad_output4.csv", 32, 0.0001, coeffList);
}

CSIM_TEST_CASE(FLOATING_POINT_4TH_ORDER)
{
    Biquad::SOSCoeffs coeffs1 = {
            .b0 = 0.311873880942884,
            .b1 = 0.496357519917410,
            .b2 = 0.311873880942884,
            .a1 = -0.254613961286827,
            .a2 = -0.290222760661470
    };

    Biquad::SOSCoeffs coeffs2 = {
            .b0 = 0.311873880942884,
            .b1 = -0.525650032497285,
            .b2 = 0.311873880942884,
            .a1 = 0.321841213460586,
            .a2 = -0.291615771228283
    };
    std::vector<Biquad::SOSCoeffs> coeffList;
    coeffList.push_back(coeffs1);
    coeffList.push_back(coeffs2);
    runFilter("./data/biquad/input/biquad_input5.csv", "./data/biquad/output/biquad_output5.csv", 32, 0.0001, coeffList);
}

/*
CSIM_TEST_CASE(FLOATING_POINT_CHIRP_BANDPASS_10ORDER)
{
    Biquad::SOSCoeffs coeffs1 = {
            .b0 = 0.311873880942884,
            .b1 = 0.496357519917410,
            .b2 = 0.311873880942884,
            .a1 = -0.254613961286827,
            .a2 = -0.290222760661470
    };

    Biquad::SOSCoeffs coeffs2 = {
            .b0 = 0.311873880942884,
            .b1 = -0.525650032497285,
            .b2 = 0.311873880942884,
            .a1 = 0.321841213460586,
            .a2 = -0.291615771228283
    };

    Biquad::SOSCoeffs coeffs3 = {
            .b0 = 0.343493885932781,
            .b1 = 0.620019614874733,
            .b2 = 0.343493885932781,
            .a1 = 0.243497677054298,
            .a2 = -0.274597155242264
    };

    Biquad::SOSCoeffs coeffs4 = {
            .b0 = 0.343493885932781,
            .b1 = -0.636113898907368,
            .b2 = 0.343493885932781,
            .a1 = -0.163819663175370,
            .a2 = -0.271593896486612
    };

    Biquad::SOSCoeffs coeffs5 = {
            .b0 = 0.471140259283514,
            .b1 = 0,
            .b2 = -0.471140259283514,
            .a1 = 0.057862803243331,
            .a2 = -0.336395040444710
    };

    std::vector<Biquad::SOSCoeffs> coeffList;
    coeffList.push_back(coeffs1);
    coeffList.push_back(coeffs2);
    coeffList.push_back(coeffs3);
    coeffList.push_back(coeffs4);
    coeffList.push_back(coeffs5);
    runFilter("./data/biquad/input/XXX", "./data/biquad/output/XXX", 32, 0.0001, coeffList);
}
*/

CSIM_TEST_SUITE_END()
