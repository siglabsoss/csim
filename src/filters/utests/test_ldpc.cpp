#include <test/unit_test.hpp>

#include <filters/ldpc_decode.hpp>
#include <filters/ldpc_encode.hpp>
#include <utils/ldpc_utils.hpp>

#include <boost/tokenizer.hpp> //For parsing data from file

CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

CSIM_TESX_CASE(LDPC_Basic)
{
    CSVMatrix* p = new CSVMatrix();
    vector<char> bytes = p->loadCSVFile("data/ldpc/code1_h.txt");

    vector<vector<uint8_t> > H;
    uint32_t rows, cols;

    p->parseCSV(rows, cols, bytes, H);

    cout << "Loaded H with rows, cols" << endl << rows << ", " << cols << endl;

    LDPCDecode decode(H, rows, cols);


    vector<int> rx = {4161 * -1, 5953 * -1, -9328 * -1, 0 * -1, -1188 * -1, 0 * -1, -1144 * -1, 0 * -1, -3925 * -1, 0 * -1, -6833 * -1, 9005 * -1, -4161 * -1, 7449 * -1, 965 * -1, -7030 * -1, 0 * -1, 0 * -1, 0 * -1, -1967 * -1, 0 * -1, 0 * -1, -9328 * -1, -3154 * -1};


    bool solved;
    size_t solved_in;

    decode.decode(rx, 10, solved, solved_in);
    cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << endl;

    decode.decode(rx, 10, solved, solved_in);
    cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << endl;

}

CSIM_TESX_CASE(LDPC_HARD_CODED_MESSAGE)
{
    CSVMatrix* p = new CSVMatrix();
    vector<char> bytes = p->loadCSVFile("data/ldpc/code2_h.txt");

    vector<vector<uint8_t> > H;
    uint32_t rows, cols;

    p->parseCSV(rows, cols, bytes, H);

    cout << "Loaded H with rows, cols" << endl << rows << ", " << cols << endl;

    LDPCDecode decode(H, rows, cols);

    //                    0,1,1,0,1,1,0,1,0
    vector<uint8_t> rx = {0,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,1,0,1,0,0,0,1,1};
    vector<int> llr = vector<int>(rx.size());

    for(size_t i = 0; i < rx.size(); i++) {
        llr[i] = rx[i] * -100; // convert to llr
    }

    bool solved;
    size_t solved_in;

    decode.decode(llr, 10, solved, solved_in);
    cout << "solved code: " << (solved?"true":"false") << " in " << solved_in << endl;

    BOOST_CHECK_EQUAL(solved, true);

    vector<uint8_t> decoded = decode.get_message();

    for(size_t i = 0; i < (cols-rows); i++) {
        BOOST_CHECK_EQUAL(rx[i], decoded[i]);
    }

}

CSIM_TESX_CASE(LDPC_ENCODE)
{
    CSVMatrix* p = new CSVMatrix();
    vector<char> g_bytes = p->loadCSVFile("data/ldpc/code2_g.txt");
    vector<char> h_bytes = p->loadCSVFile("data/ldpc/code2_h.txt");


    vector<vector<uint8_t> > G;
    uint32_t g_rows, g_cols;
    p->parseCSV(g_rows, g_cols, g_bytes, G);

    vector<vector<uint8_t> > H;
    uint32_t h_rows, h_cols;
    p->parseCSV(h_rows, h_cols, h_bytes, H);




    vector<uint8_t> u = {0,1,1,0,1,1,0,1,0};
    vector<uint8_t> cw;

    LDPCEncode encode(G, g_rows, g_cols);

    cw = encode.encode(u);

    cout << "gotback cw " << cw.size() << endl;

    for(size_t i = 0; i < cw.size(); ++i) {
        cout << (int)cw[i] << ", ";
    }

    cout << endl;


}


CSIM_TEST_CASE(LDPC_ENCODE_COOKED)
{
    std::string gstring( "1,1,1,1,1\n0,1,0,0,0\n0,1,1,0,0\n" );
    std::vector<char> g_bytes( gstring.begin(), gstring.end() );

    vector<vector<uint8_t> > G;
    uint32_t g_rows, g_cols;
    CSVMatrix* p = new CSVMatrix();
    p->parseCSV(g_rows, g_cols, g_bytes, G);

    cout << "Loaded G with rows, cols" << endl << g_rows << ", " << g_cols << endl;

    vector<uint8_t> u = {1,0,1};

    LDPCEncode encode(G, g_rows, g_cols);

    vector<uint8_t> cw;

    cw = encode.encode(u);


}

//CSIM_TEST_CASE(CSV_Parse)
//{
//    CSVMatrix* p = new CSVMatrix();
//
//    vector<char> bytes;
//    bytes = p->loadCSVFile("data/ldpc/mat1.txt");
//
//    uint32_t rows, cols;
//
//    vector<vector<uint8_t> > H;
//
//    p->parseCSV(rows, cols, bytes, H);
//
//    cout << "H rows " << H.size() << " H cols " << H[0].size() << endl;
//
//    cout << endl << endl;
//
//    for (auto it = H.begin(); it != H.end(); ++it) {
//        for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
//            cout << (int) *it2 << ",";
//        }
//        cout << endl;
//    }
//}

CSIM_TEST_SUITE_END()
