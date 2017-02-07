#include <utils/utils.hpp>
#include <filters/ldpc_decoder.hpp>
#include <utils/ldpc_utils.hpp>

static void runLDPCDecodeTest(
    const std::vector<SLFixedPoint<LDPC_LLR_FORMAT> >& rxCodeWord,
    const std::vector<bool>                          & validMessage,
    const std::string                                & HFileName)
{
    std::vector<std::vector<bool> > H = LDPCUtils::getBitArrayFromCSV(HFileName);

    size_t rows = H.size();
    size_t cols = H[0].size();
    std::cout << "Loaded H with rows, cols " << rows << ", " << cols << std::endl;

    LDPCDecoder decode(H, false /* no early exit */);

    decode.setDebug(true);

    filter_io_t sample;

    for (size_t i = 0; i < rxCodeWord.size(); i++) {
        sample = rxCodeWord[rxCodeWord.size() - 1 - i];
        decode.input(sample);
    }

    for (size_t i = 0; i < validMessage.size() / 2; ++i) {
        decode.tick();
        bool didOutput = decode.output(sample);

        if (didOutput == false) {
            std::cout << "LDPC decoder did not output when expected!" <<
            std::endl;
            return;
        }

        if (sample.type != IO_TYPE_BIT) {
            std::cout << "LDPC decoder did not output expected data type!" <<
            std::endl;
            return;
        }

        // if (sample.bit != validMessage[validMessage.size() / 2 - 1 - i]) {
        //     std::cout << "Bit #" << i << " does not match!" << std::endl;
        // }
    }
}

int main(int argc, char *argv[]) {
    bool gotLLRFile = false, gotHFile = false, gotCWFile = false;
    std::string LLRFile, HFile, CWFile;
    int c;

    while ((c = getopt(argc, argv, "l:h:c:")) != -1) {
        switch (c) {
        case 'l':
            LLRFile    = std::string(optarg);
            gotLLRFile = true;
            break;

        case 'h':
            HFile    = std::string(optarg);
            gotHFile = true;
            break;

        case 'c':
            CWFile    = std::string(optarg);
            gotCWFile = true;
            break;
        }
    }

    if ((gotLLRFile != true) || (gotHFile != true) || (gotCWFile != true)) {
        std::cout << "Usage: " << argv[0] <<
        " -l <llr filename> -h <H matrix filename> -c <codeword filename>" <<
        std::endl;
        return 1;
    }

    std::vector<ComplexDouble> LLRs = utils::readComplexFromCSV<ComplexDouble>(
        LLRFile);
    std::vector<ComplexDouble> message = utils::readComplexFromCSV<ComplexDouble>(
        CWFile);

    assert(LLRs.size() > 0);
    assert(message.size() > 0);

    std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > rxCodeWord(LLRs.size());
    std::vector<bool> validMessage(message.size());

    for (size_t i = 0; i < rxCodeWord.size(); ++i) {
        rxCodeWord[i] = LLRs[i].real() / pow(2, LDPC_LLR_WL - LDPC_LLR_IWL);
    }

    for (size_t i = 0; i < validMessage.size(); ++i) {
        validMessage[i] = (static_cast<int>(message[i].real()) == 1);
    }

    std::cout << "Loaded " << rxCodeWord.size() << " LLR values" << std::endl;

    runLDPCDecodeTest(rxCodeWord, validMessage, HFile);
}
