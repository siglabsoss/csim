#include <test/unit_test.hpp>

#include <filters/ldpc_decode.hpp>
#include <filters/ldpc_encode.hpp>
#include <utils/ldpc_utils.hpp>

#include <boost/tokenizer.hpp> //For parsing data from file

CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

CSIM_TEST_CASE(LDPC_Basic)
{
    CSVBitMatrix p;
    std::vector<char> bytes = p.loadCSVFile("data/ldpc/code1_h.txt");

    std::vector<std::vector<bool> > H;

    p.parseCSV(bytes, H);

    size_t rows = H.size();
    size_t cols = H[0].size();
    std::cout << "Loaded H with rows, cols" << std::endl << rows << ", " << cols << std::endl;

    LDPCDecode decode(H);

    std::vector<int> rx = {4161 * -1, 5953 * -1, -9328 * -1, 0 * -1, -1188 * -1, 0 * -1, -1144 * -1, 0 * -1, -3925 * -1, 0 * -1, -6833 * -1, 9005 * -1, -4161 * -1, 7449 * -1, 965 * -1, -7030 * -1, 0 * -1, 0 * -1, 0 * -1, -1967 * -1, 0 * -1, 0 * -1, -9328 * -1, -3154 * -1};


    bool solved;
    size_t solved_in;

    decode.decode(rx, 10, solved, solved_in);
    std::cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << std::endl;

    decode.decode(rx, 10, solved, solved_in);
    std::cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << std::endl;

}

CSIM_TEST_CASE(LDPC_HARD_CODED_MESSAGE)
{
    CSVBitMatrix p;
    std::vector<char> bytes = p.loadCSVFile("data/ldpc/code2_h.txt");

    std::vector<std::vector<bool> > H;

    p.parseCSV(bytes, H);

    size_t rows = H.size();
    size_t cols = H[0].size();
    std::cout << "Loaded H with rows, cols" << std::endl << rows << ", " << cols << std::endl;

    LDPCDecode decode(H);

    //                    0,1,1,0,1,1,0,1,0
    std::vector<bool> rx = {0,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,1,0,1,0,0,0,1,1};
    std::vector<int> llr = std::vector<int>(rx.size());

    for(size_t i = 0; i < rx.size(); i++) {
        llr[i] = rx[i] * -100; // convert to llr
    }

    bool solved;
    size_t solved_in;

    decode.decode(llr, 10, solved, solved_in);
    std::cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << std::endl;

    BOOST_CHECK_EQUAL(solved, true);

    std::vector<bool> decoded = decode.get_message();

    for(size_t i = 0; i < (cols-rows); i++) {
        BOOST_CHECK_EQUAL(rx[i], decoded[i]);
    }

}

CSIM_TEST_CASE(LDPC_ENCODE)
{
    //the message size is 9-bits, but the input is byte aligned.
    //inputting 16 bits but we will only get one 9-bit message encoded
    //using the first 9 bits
    std::vector<uint8_t> u = {0b01101101, 0b00000000};

    //in this test the codeword size is 24-bits, so we can just make comparisons
    //in a byte-wise fashion
    std::vector<uint8_t> expectedCodeWord = {0b01101101, 0b01110111, 0b10100011};

    CSVBitMatrix p;
    std::vector<char> g_bytes = p.loadCSVFile("data/ldpc/code2_g.txt");

    std::vector<std::vector<bool> > G;
    p.parseCSV(g_bytes, G);

    LDPCEncode encoder(G);


    filter_io_t data;
    for (size_t i = 0; i < u.size(); i++) {
        data.type = IO_TYPE_BYTE;
        data.byte = u[i];
        encoder.input(data);
        encoder.tick();
    }

    std::vector<uint8_t> output;
    while (encoder.output(data)) {
        BOOST_CHECK_EQUAL(data.type, IO_TYPE_BYTE);
        output.push_back(data.byte);
    }
    BOOST_CHECK(output.size() == 3);
    for (size_t i = 0; i < output.size(); i++) {
        BOOST_CHECK_EQUAL(output[i], expectedCodeWord[i]);
    }
}

CSIM_TEST_CASE(LDPC_ENCODE_COOKED)
{
    std::string gstring( "1,1,1,1,1\n0,1,0,0,0\n0,1,1,0,0\n" );
    std::vector<char> g_bytes( gstring.begin(), gstring.end() );

    std::vector<std::vector<bool> > G;
    CSVBitMatrix p;
    p.parseCSV(g_bytes, G);

    size_t g_rows = G.size();
    size_t g_cols = G[0].size();
    std::cout << "Loaded G with rows, cols" << std::endl << g_rows << ", " << g_cols << std::endl;

    std::vector<bool> u = {1,0,1};

    LDPCEncode encode(G);

    std::vector<bool> cw;
    std::vector<bool> expected = {1,0,0,1,1};

    cw = encode.encode(u);

    for(size_t i = 0; i < cw.size(); i++)
    {
        BOOST_CHECK_EQUAL(cw[i], expected[i]);
    }


}

//CSIM_TEST_CASE(CSV_Parse)
//{
//    CSVBitMatrix p;
//
//    std::vector<char> bytes;
//    bytes = p.loadCSVFile("data/ldpc/mat1.txt");
//
//    uint32_t rows, cols;
//
//    std::vector<std::vector<bool> > H;
//
//    p.parseCSV(rows, cols, bytes, H);
//
//    std::cout << "H rows " << H.size() << " H cols " << H[0].size() << std::endl;
//
//    std::cout << std::endl << std::endl;
//
//    for (auto it = H.begin(); it != H.end(); ++it) {
//        for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
//            std::cout << (int) *it2 << ",";
//        }
//        std::cout << std::endl;
//    }
//}

CSIM_TEST_SUITE_END()
