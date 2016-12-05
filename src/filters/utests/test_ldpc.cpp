#include <test/unit_test.hpp>

#include <filters/ldpc_decoder.hpp>
#include <filters/ldpc_encode.hpp>
#include <utils/ldpc_utils.hpp>

#include <boost/tokenizer.hpp> //For parsing data from file

CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

static void runLDPCDecodeTest(const std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > &rxCodeWord, const std::vector<bool> &validMessage, const std::string &HFileName)
{
    CSVBitMatrix p;
    std::vector<char> bytes = p.loadCSVFile(HFileName);

    std::vector<std::vector<bool> > H;

    p.parseCSV(bytes, H);

    size_t rows = H.size();
    size_t cols = H[0].size();
    std::cout << "Loaded H with rows, cols " << rows << ", " << cols << std::endl;

    LDPCDecoder decode(H);

    filter_io_t sample;
    for (size_t i = 0; i < rxCodeWord.size(); i++) {
        sample = rxCodeWord[rxCodeWord.size() - 1 - i];
        decode.input(sample);
    }

    for (size_t i = 0; i < validMessage.size(); ++i) {
        decode.tick();
        bool didOutput = decode.output(sample);
        BOOST_CHECK_EQUAL(didOutput, true);
        BOOST_CHECK_EQUAL(sample.type, IO_TYPE_BIT);
        BOOST_CHECK_MESSAGE(sample.bit == validMessage[validMessage.size() - 1 - i], "Bit #" << i << " does not match");
    }
}

static void runLDPCEncodeTest(const std::vector<bool> &msg, const std::vector<bool> &cw, const std::string &GFileName)
{
    CSVBitMatrix p;
    std::vector<char> g_bytes = p.loadCSVFile(GFileName);

    std::vector<std::vector<bool> > G;
    p.parseCSV(g_bytes, G);

    LDPCEncode encoder(G);

    filter_io_t data;

    //Feed in LSB first
    for (ssize_t i = msg.size() - 1; i >= 0; --i) {
        data.type = IO_TYPE_BIT;
        data.bit = msg[i];
        encoder.input(data);
        encoder.tick();
    }

    std::vector<bool> output(cw.size());
    ssize_t idx = output.size() - 1;
    while (encoder.output(data)) {
        BOOST_CHECK_EQUAL(data.type, IO_TYPE_BIT);
        BOOST_CHECK(idx >= 0);
        output[idx] = data.bit;
        --idx;
    }
    BOOST_CHECK(idx == -1);
    for (size_t i = 0; i < output.size(); i++) {
        BOOST_CHECK_MESSAGE(output[i] == cw[i], "Encoder output bit #" << i << " does not match");
    }
}

static void runLDPCLoopbackTest(const std::vector<bool> &msg, const std::string &GFileName, const std::string &HFileName)
{
    CSVBitMatrix g;
    std::vector<char> g_bytes = g.loadCSVFile(GFileName);

    std::vector<std::vector<bool> > G;
    g.parseCSV(g_bytes, G);

    LDPCEncode encoder(G);

    filter_io_t data;

    //Feed in LSB first
    for (ssize_t i = msg.size() - 1; i >= 0; --i) {
        data.type = IO_TYPE_BIT;
        data.bit = msg[i];
        encoder.input(data);
        encoder.tick();
    }

    std::vector<bool> output(648); //get codeword size differently
    ssize_t idx = output.size() - 1;
    while (encoder.output(data)) {
        BOOST_CHECK_EQUAL(data.type, IO_TYPE_BIT);
        //BOOST_REQUIRE(idx >= 0);
        assert(idx >= 0);
        output[idx] = data.bit;
        --idx;
    }

    std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > rxCodeWord(msg.size()*2);
    for (size_t i = 0; i < rxCodeWord.size(); i++) {
        if (output[i] == 1) {
            rxCodeWord[i] = -5.0;
        } else {
            rxCodeWord[i] = 5.0;
        }
    }

    CSVBitMatrix h;
    std::vector<char> bytes = h.loadCSVFile(HFileName);

    std::vector<std::vector<bool> > H;

    h.parseCSV(bytes, H);

    size_t rows = H.size();
    size_t cols = H[0].size();
    std::cout << "Loaded H with rows, cols " << rows << ", " << cols << std::endl;

    LDPCDecoder decode(H);

    filter_io_t sample;
    for (size_t i = 0; i < rxCodeWord.size(); i++) {
        sample = rxCodeWord[rxCodeWord.size() - 1 - i];
        decode.input(sample);
    }

    for (size_t i = 0; i < msg.size(); ++i) {
        decode.tick();
        bool didOutput = decode.output(sample);
        BOOST_CHECK_EQUAL(didOutput, true);
        BOOST_CHECK_EQUAL(sample.type, IO_TYPE_BIT);
        BOOST_CHECK_MESSAGE(sample.bit == msg[msg.size() - 1 - i], "Bit #" << msg.size() - 1 - i << " does not match");
    }
}

CSIM_TEST_CASE(LDPC_DECODE_FLASH_SUMMIT_EXAMPLE)
{
    //This is the example parity matrix and codeword from
    //https://dl.dropboxusercontent.com/content_link/w8n7SI6nL2TSa8R5kABYL32eBYpGzACL8g8DTxGmTNukqbENnzoelQkIQQpls0xp/file

    std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > llrs = {-9.0, -7.0, -12.0, -4.0, 7.0, 10.0, -11.0};
    std::vector<bool> message = {1, 0, 1};

    runLDPCDecodeTest(llrs, message, "data/ldpc/code3_h.txt");
}

CSIM_TEST_CASE(LDPC_80211n_DECODE_MATLAB_GENERATED_CODEWORD)
{
    std::vector<bool> message          = {0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,1,1,0,1,1,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0};
    std::vector<bool> expectedCodeWord = {0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,1,1,0,1,1,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,0,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,0,1,1,1,0,0,1,0,1,1,1,0,1,0,1,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,0,1,1,0,0,1,1,1,0,1,0,1,0,0,0,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,1,0,1,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,0,0,1,1,1,1,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,0,0,0,0,0,1,0,1,0,0,1,1,1,1,0,0,1,0,1,0,1,1,0,0,1,0,0,1,1,1,0,0,0,0,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,1,0,1,1,0,1,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,0,0,0,1,0,1,0,0,1,1,0,1,0,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,1,1};
    std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > llr(expectedCodeWord.size());

    std::default_random_engine randomEngine;
    std::normal_distribution<double> distribution(5, 1.0);
    for (size_t i = 0; i < llr.size(); i++) {
        double randLLR = distribution(randomEngine);
        if (randLLR < 0.0) { //don't allow negative values
            randLLR = 0.0;
        }
        if (expectedCodeWord[i] == 0) {
            llr[i] = randLLR;
        } else {
            llr[i] = -randLLR; //1's are represented with negative LLRs
        }
    }

    //Flip the bits as the following indexes
    std::vector<size_t> errorIdxs = {123, 357, 183, 235, 260, 042, 196, 613, 546, 640, 13, 155};
    for (size_t i = 0; i < errorIdxs.size(); i++) {
        size_t idx = errorIdxs[i];
        llr[idx] = -llr[idx].to_double();
    }

    runLDPCDecodeTest(llr, message, "data/ldpc/80211n_H_648_12.csv");
}

CSIM_TEST_CASE(LDPC_80211N_DECODE_GENERATED_FROM_PYTHON)
{
    std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > llrs = {-4.063600000000001, 4.552560000000001, 3.9589440000000007, 3.8727160000000005, 4.28068, -4.07144, -3.7298360000000006, 2.8705879999999993, -4.34212, 5.93444, 1.8320480000000008, -3.0318000000000005, -3.826492, -3.486452, 3.7896239999999994, -3.5601280000000006, 2.395832, -5.36596, 4.072440000000001, -3.9036879999999994, 3.623452000000001, 4.88876, 3.0365799999999994, -5.2947999999999995, -4.539159999999999, -2.792352, -3.5672199999999994, 2.953139999999999, -3.8962599999999994, 3.1276320000000006, -5.248480000000002, 5.064000000000002, -3.4234039999999992, 5.261959999999999, 3.4315799999999994, -4.046520000000001, -4.12448, 4.579360000000002, 4.69384, -3.8215319999999995, 4.12388, 2.7254039999999997, -4.9449200000000015, -4.5589200000000005, 3.5367079999999995, 3.5358879999999995, 3.273824, 4.972600000000001, -3.742764, -3.747684, -5.21816, 4.34964, 5.42332, 3.4944079999999995, -4.6642399999999995, 5.731840000000002, -5.25024, 5.300799999999999, 3.847424, 3.4381240000000006, 3.7534, 3.642944, -4.138919999999999, 4.25968, -4.1118, -4.30708, 2.068548, -3.433064, -5.87484, -4.689840000000001, 5.036559999999999, 3.4855040000000006, -3.515588, 3.887052, 4.798919999999997, 3.2295680000000004, 3.8814960000000003, 5.01636, 4.342120000000001, 3.810339999999999, 3.6062039999999995, 3.6933760000000007, 2.9104919999999996, -3.4785799999999996, 4.098279999999999, 4.8633999999999995, -2.570984, 4.5672, -2.6402959999999998, -2.4638639999999996, 3.0884120000000004, 2.2309040000000007, -4.2002799999999985, -4.144399999999999, -3.806447999999999, -5.411639999999999, -2.001419999999999, -6.2867999999999995, 3.2351639999999997, 4.494480000000001, -3.8283760000000004, -3.3157639999999997, -3.3751759999999997, -3.578644, -4.928000000000002, -3.180976, -3.4524240000000006, -4.9347199999999996, -4.425799999999998, -5.856, 4.584160000000002, 4.63256, 3.3056439999999996, -4.6924399999999995, -2.3889519999999997, -4.28756, 5.166479999999999, -5.246679999999999, -3.869416, -4.832480000000001, 3.743432, 4.114960000000001, 3.6841759999999995, -3.6870919999999994, -3.779664000000001, -4.11092, 3.725748, 3.9614999999999996, -4.17292, -3.3509519999999995, 4.880999999999999, -4.271000000000002, -5.879559999999999, -4.750919999999999, -4.719879999999999, -2.7840760000000015, -2.8951639999999994, -4.188559999999999, -5.307079999999999, 5.56404, -4.295799999999999, 4.757000000000001, 3.677492, 4.03284, 4.870400000000001, -3.9606159999999995, -3.2191360000000007, -5.835319999999999, -3.348388, 4.53444, 5.334920000000001, 4.50604, 4.161880000000001, 3.9963879999999996, 3.6534560000000003, 3.691955999999999, -3.0841159999999994, -3.322152, -3.78016, -3.7374840000000007, -5.89872, 4.65232, -3.561992, -4.259360000000001, -2.5249399999999995, 5.801880000000001, 2.08216, 3.779692, -4.3889999999999985, 3.232592, -3.6574240000000002, 3.7888, 3.707508, 3.069484000000001, -5.544080000000001, 4.959440000000001, 3.2341, 3.8079199999999997, 4.366160000000001, 3.4339960000000005, -4.086239999999999, -2.879124, 3.0072799999999993, -2.90566, 5.598079999999999, 4.228640000000001, 2.6170279999999995, -3.732412, -4.22608, 5.841560000000003, -5.225400000000001, 3.0663400000000003, 5.41148, -4.947520000000001, 3.355008, 3.6124679999999993, 4.513039999999999, -1.4827079999999992, 4.001040000000001, -4.176680000000001, -4.479479999999998, 2.827847999999999, 2.6644, -3.261948, 4.3211200000000005, 4.573039999999999, -5.153079999999998, -3.840588000000001, 4.0068399999999995, -2.518072, -3.4756640000000005, -4.595720000000002, 4.674479999999999, -3.563563999999999, 4.75944, 3.052196000000001, -2.7487080000000006, -3.4719999999999995, -3.5848399999999994, 4.065480000000001, -5.527440000000001, 4.465679999999998, -3.502284, -2.659604, 4.3333200000000005, 3.237548, -4.492599999999998, 3.5680280000000004, -5.22308, -4.990519999999999, 2.970408, 5.68652, 2.9661919999999995, -6.09624, -3.729288000000001, 4.369679999999997, 4.178799999999999, 3.3463279999999997, -3.929928, 3.310679999999999, 4.5434, 1.1692320000000003, 4.76576, -4.19388, -3.689364, 3.790459999999999, 4.267720000000001, 4.156279999999999, -4.04104, -3.822724, -2.2326440000000005, -3.065032000000001, -3.885388, 3.7410479999999993, -3.320084, -3.5143, 5.948039999999999, -2.7587119999999996, 4.74112, -4.30596, -3.394412, 3.6643400000000006, 4.590759999999999, -3.093344, -4.134560000000001, 3.506499999999999, 3.367480000000001, -4.49348, -5.04696, -4.009080000000001, -3.2489959999999996, -5.0050799999999995, 2.768591999999999, -4.729999999999999, 2.752904, -4.1931199999999995, 3.9119720000000004, -3.5319959999999995, 3.3842, -4.682559999999999, -3.5268599999999997, 4.673159999999999, -3.6565879999999997, -4.79096, -4.415840000000001, -6.415119999999999, -2.8956239999999998, -4.6349599999999995, 4.09956, -3.2247079999999997, 5.2720400000000005, -4.2484800000000025, 4.584920000000001, -4.322680000000001, -2.9727600000000005, -4.6856, -3.423116, 2.883447999999999, 6.105119999999999, -3.4381239999999997, 1.6307440000000002, -4.273639999999999, 4.248000000000001, 3.5899240000000003, 3.7644719999999996, 4.001599999999999, 4.376120000000001, 4.413160000000002, -4.547720000000002, -4.456800000000001, -3.572307999999999, 3.837832, -3.8371919999999995, -3.7651520000000005, -4.946480000000002, -5.132040000000001, -3.68742, -5.725439999999999, 3.8445, 4.72492, 3.0835839999999997, 4.277280000000001, -4.02576, -4.067239999999999, 2.7180999999999993, 5.00428, -4.72204, 4.25696, 3.364104, -3.7080159999999998, 4.82824, -3.8568759999999997, 4.597039999999999, 3.567124, 4.334320000000001, 4.9204799999999995, -2.774487999999999, 5.376999999999999, -4.416760000000001, 4.986280000000002, -4.463239999999999, -4.190120000000001, -2.6375, -4.62252, 3.763364, -3.098812, -3.698392, 3.6032840000000004, 4.790920000000001, 4.655879999999999, -3.683904, -2.8487439999999995, -4.8554, 4.71632, 4.007, 4.1831999999999985, 4.39376, -3.8465799999999986, -3.513352, 5.646159999999999, 3.883312, 3.689812, 3.1575, -2.243348, 3.606688, -4.403999999999999, -3.413660000000001, 2.169712000000001, 2.8951360000000004, 4.66212, -3.981208, 4.187079999999999, 3.6309799999999997, -3.415976, 4.199080000000001, 5.441320000000001, 5.2151999999999985, 3.6965120000000002, 5.354240000000001, 3.209052, 4.668160000000001, -3.493264, -5.06372, -4.888000000000001, 5.114960000000001, 3.213472, 3.441064, 3.9779599999999986, 5.06696, -2.9802599999999995, -5.189, -3.47414, 5.8209599999999995, 4.128319999999999, 4.114519999999999, 4.2305600000000005, -3.70834, 5.304359999999997, -2.754844, -4.678239999999999, -3.773192, -6.483440000000001, 4.746480000000002, -3.8778840000000003, 5.57048, -3.3901359999999996, -2.547652, -3.120328, 2.792391999999999, -5.035640000000001, 3.5949360000000006, -3.709876, -2.0672040000000003, 3.7387040000000007, 3.6656879999999994, -4.95276, -3.3114120000000002, -5.617280000000002, -2.7441200000000006, 4.345160000000001, -3.1394280000000006, 4.506719999999999, 3.9649760000000005, -4.017800000000001, -3.760504, 4.697040000000001, -2.448328, 2.2963839999999998, 3.326764, -3.5585279999999995, 2.9164479999999995, 4.002280000000001, 4.99836, 3.5374479999999995, 3.7628679999999997, 5.053799999999999, -4.24352, 4.609919999999998, 3.6921160000000004, -4.604119999999999, -4.464839999999998, 3.0960040000000006, -4.14816, -4.915520000000002, 3.5642040000000006, -3.4430360000000007, -4.614759999999999, 5.99484, 1.9570920000000003, 2.642556, -4.223919999999999, 3.156384, 4.632760000000003, -3.6973640000000003, -4.252320000000003, 4.18248, -4.28844, 5.3409200000000014, -4.416560000000001, -3.163667999999999, -5.52676, -4.25692, 3.2664679999999997, -5.005279999999999, -4.65212, -2.8452560000000005, 4.32388, 5.72204, -4.075080000000001, -3.4264399999999995, 2.54982, -3.9758359999999997, 3.995687999999999, -3.9239319999999993, -5.0926800000000005, -5.448800000000001, -6.002559999999999, 3.2648519999999994, -3.9705120000000007, -3.2255959999999995, -1.8289960000000003, 6.048039999999999, 4.59088, -3.4432440000000004, -5.25576, 2.879160000000001, 2.7168039999999993, 2.457172, -3.6745119999999996, 4.08276, 4.575640000000002, 4.02908, 3.8049399999999998, -4.98916, 3.8682359999999987, 3.799716, -3.6198839999999994, -3.7846319999999998, 4.121479999999999, -5.17348, 5.568879999999999, 3.6586079999999996, 5.37232, 4.843, -4.9222800000000015, -3.94874, 3.011532, 3.7518200000000004, 5.299599999999999, -3.78226, 2.916883999999999, 2.0244199999999997, -3.7297399999999996, -3.909716, 4.09952, 2.862108, 4.878160000000002, 5.252559999999999, -1.79276, -2.6917159999999996, 4.578079999999999, 3.91164, -4.477080000000002, -4.084840000000001, -4.465920000000001, -4.467279999999997, 4.11792, 3.9194359999999997, -3.3778120000000005, 2.493472, 5.081640000000002, 3.7090599999999996, 5.4559999999999995, -2.4194919999999995, -3.8049, -3.2028999999999996, -4.543800000000002, -4.8058000000000005, 5.684640000000001, -3.3523119999999995, 2.302364, -4.7991199999999985, 2.6351919999999995, -3.2301399999999996, 3.1326999999999994, 3.7099600000000006, -5.745840000000002, 4.21924, 4.770720000000002, -4.338960000000001, -4.899039999999999, -4.362840000000001, -3.9117119999999996, -2.2766079999999995, 3.6279240000000006, 3.609208, -4.030599999999999, 3.4791600000000003, 4.022079999999998, -3.604488, 3.394648, -4.375560000000001, 3.937708, 4.5284, 4.68004, 4.880600000000002, -3.5815839999999985, -4.152239999999999, -3.286956, -3.310052, -2.8065200000000003, 3.938308, -4.04228, 3.8773800000000005, 3.984268, 2.8203879999999995, 5.274399999999999, -4.36792, 3.1077680000000005, -4.37228, -5.731719999999999, -5.5556, -2.7367600000000003, 3.4402919999999986, -3.608248, -3.209756, -4.266639999999999, 4.23728, -4.688800000000001, -3.220568, 4.45168, -4.538279999999999, -7.28096, -4.501239999999999, -4.196759999999999, -2.6399120000000003, 4.291159999999999, 2.9109479999999994, -4.334759999999998, 2.931, 4.0972800000000005, -4.21024, 3.0313119999999993, -3.162992, 5.05476, 3.0432239999999995, -4.23564, 4.300039999999999, -3.907844, -5.494560000000001, 4.655519999999998, -3.866407999999999, -2.6870320000000003, -4.554719999999999, 4.766839999999999, 4.624040000000001, -4.136520000000003, -2.5213440000000005, 4.655799999999999, -2.891976, 2.30846, -4.32752, 6.31564, -4.790759999999997, 4.95688, -2.6578280000000003, -3.9170000000000003, -5.176919999999998, -5.288319999999998, -4.909279999999998, 4.526879999999999, -4.180160000000002, 4.190080000000001, 5.260280000000001, 4.52204, -4.95692, -3.8018359999999998, 3.756424, -3.986468, 3.5321600000000006, 3.203028, -5.425720000000001, -4.351399999999999, -4.29972, -4.1346, -3.82764, -3.414116000000001, -3.7860800000000006};
    for (size_t i = 0; i < llrs.size(); i++) {
        llrs[i] = -(llrs[i].to_double());
    }
    std::vector<bool> message = {0,1,1,1,1,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,1,1,0,0,0,0,1,0,1,0,1,0,1,1,0,0,1,1,0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,0,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0,0,0,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,0,1,0,1,1,1,0,0,1,0,1,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,0,1,1,0,1,0,0,1,1,1,0,0,1,1,1,0,1,1,1,1,0,0,1,1,1,0,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,0,1,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,1,1,0,1,0,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,0};
    runLDPCDecodeTest(llrs, message, "data/ldpc/80211n_H_648_12.csv");
}

CSIM_TEST_CASE(LDPC_LOOPBACK)
{
    std::vector<bool> message =          {0,1,0,1,0,0,0,0, 1,0,1,1,0,0,0,0, 0,0,1,0,1,0,0,0, 0,0,1,0,0,1,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,1,1,0, 1,1,0,0,0,0,1,0, 1,1,1,1,0,0,0,1, 1,1,0,1,0,0,0,0, 1,0,1,1,0,0,0,1, 1,1,1,0,1,0,1,0, 1,0,1,1,1,1,0,0, 1,0,1,0,1,0,1,0, 0,1,1,1,1,0,0,1, 0,1,1,1,1,0,0,1, 1,0,1,1,0,0,0,1, 1,1,1,1,0,0,0,1, 0,1,1,0,0,1,0,1, 0,0,0,1,1,0,0,0, 0,0,0,1,1,1,0,0, 1,1,0,1,0,1,0,0, 1,0,0,0,1,1,1,0, 1,0,1,0,1,0,0,1, 1,1,1,1,1,1,1,0, 1,1,1,0,0,0,0,0, 1,0,1,0,0,0,1,1, 1,1,0,0,1,0,0,1, 0,0,0,1,0,0,1,1, 0,1,0,0,1,1,1,0, 0,1,1,1,1,0,0,1, 0,0,1,1,0,0,1,1, 0,1,1,1,0,0,1,0, 0,0,1,0,1,0,1,1, 1,0,0,0,0,0,1,0, 1,1,0,1,0,0,1,1, 1,1,0,1,0,0,1,1, 1,1,1,1,0,1,1,1, 0,0,1,0,1,0,0,0, 0,0,0,1,0,0,1,0, 0,1,0,1,1,0,0,0, 0,0,0,1};
    runLDPCLoopbackTest(message, "data/ldpc/80211n_G_648_12.csv", "data/ldpc/80211n_H_648_12.csv");
}

CSIM_TEST_CASE(LDPC_80211N_ENCODE_MATLAB_GENERATED_CODEWORD)
{
    std::vector<bool> message          = {0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,1,1,0,1,1,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0};
    std::vector<bool> expectedCodeWord = {0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0,1,1,0,1,1,1,1,1,1,1,0,0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1,1,1,0,0,1,1,0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,1,1,0,1,1,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,0,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,0,1,1,1,0,0,1,0,1,1,1,0,1,0,1,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,0,1,1,0,0,1,1,1,0,1,0,1,0,0,0,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,1,0,1,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,1,0,0,1,1,1,1,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,0,0,0,0,0,1,0,1,0,0,1,1,1,1,0,0,1,0,1,0,1,1,0,0,1,0,0,1,1,1,0,0,0,0,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,0,1,0,0,0,0,1,0,1,1,0,1,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,0,0,0,1,0,1,0,0,1,1,0,1,0,0,1,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,1,1};

    runLDPCEncodeTest(message, expectedCodeWord, "data/ldpc/80211n_G_648_12.csv");
}

CSIM_TEST_SUITE_END()
