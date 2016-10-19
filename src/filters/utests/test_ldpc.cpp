#include <test/unit_test.hpp>

#include <filters/ldpc_decode.hpp>
#include <filters/ldpc_encode.hpp>
#include <utils/ldpc_utils.hpp>

#include <boost/tokenizer.hpp> //For parsing data from file

CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

CSIM_TESX_CASE(LDPC_Basic)
{
    CSVBitMatrix* p = new CSVBitMatrix();
    std::vector<char> bytes = p->loadCSVFile("data/ldpc/code1_h.txt");

    std::vector<std::vector<bool> > H;
    uint32_t rows, cols;

    p->parseCSV(rows, cols, bytes, H);

    std::cout << "Loaded H with rows, cols" << std::endl << rows << ", " << cols << std::endl;

    LDPCDecode decode(H, rows, cols);


    std::vector<int> rx = {4161 * -1, 5953 * -1, -9328 * -1, 0 * -1, -1188 * -1, 0 * -1, -1144 * -1, 0 * -1, -3925 * -1, 0 * -1, -6833 * -1, 9005 * -1, -4161 * -1, 7449 * -1, 965 * -1, -7030 * -1, 0 * -1, 0 * -1, 0 * -1, -1967 * -1, 0 * -1, 0 * -1, -9328 * -1, -3154 * -1};


    bool solved;
    size_t solved_in;

    decode.decode(rx, 10, solved, solved_in);
    std::cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << std::endl;

    decode.decode(rx, 10, solved, solved_in);
    std::cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << std::endl;

}

CSIM_TESX_CASE(LDPC_HARD_CODED_MESSAGE)
{
    CSVBitMatrix* p = new CSVBitMatrix();
    std::vector<char> bytes = p->loadCSVFile("data/ldpc/code2_h.txt");

    std::vector<std::vector<bool> > H;
    uint32_t rows, cols;

    p->parseCSV(rows, cols, bytes, H);

    std::cout << "Loaded H with rows, cols" << std::endl << rows << ", " << cols << std::endl;

    LDPCDecode decode(H, rows, cols);

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
    CSVBitMatrix* p = new CSVBitMatrix();
    std::vector<char> g_bytes = p->loadCSVFile("data/ldpc/code2_g.txt");
    std::vector<char> h_bytes = p->loadCSVFile("data/ldpc/code2_h.txt");


    std::vector<std::vector<bool> > G;
    uint32_t g_rows, g_cols;
    p->parseCSV(g_rows, g_cols, g_bytes, G);

    std::vector<std::vector<bool> > H;
    uint32_t h_rows, h_cols;
    p->parseCSV(h_rows, h_cols, h_bytes, H);




    std::vector<bool> u = {0,1,1,0,1,1,0,1,0};
    std::vector<bool> cw;
    std::vector<bool> expected = {0,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,1,0,1,0,0,0,1,1};

    LDPCEncode encode(G);

    cw = encode.encode(u);

    std::cout << "gotback cw " << cw.size() << std::endl;

    for(size_t i = 0; i < cw.size(); ++i) {
        std::cout << (int)cw[i] << ", ";
        BOOST_CHECK_EQUAL(cw[i], expected[i]);
    }

    std::cout << std::endl;


}


CSIM_TESX_CASE(LDPC_ENCODE_COOKED)
{
    std::string gstring( "1,1,1,1,1\n0,1,0,0,0\n0,1,1,0,0\n" );
    std::vector<char> g_bytes( gstring.begin(), gstring.end() );

    std::vector<std::vector<bool> > G;
    uint32_t g_rows, g_cols;
    CSVBitMatrix* p = new CSVBitMatrix();
    p->parseCSV(g_rows, g_cols, g_bytes, G);

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
//    CSVBitMatrix* p = new CSVBitMatrix();
//
//    std::vector<char> bytes;
//    bytes = p->loadCSVFile("data/ldpc/mat1.txt");
//
//    uint32_t rows, cols;
//
//    std::vector<std::vector<bool> > H;
//
//    p->parseCSV(rows, cols, bytes, H);
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
