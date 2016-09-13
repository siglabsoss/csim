#include <test/unit_test.hpp>

#include <filters/ldpc_decode.hpp>
#include <utils/ldpc_utils.hpp>

#include <boost/tokenizer.hpp> //For parsing data from file

CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

CSIM_TEST_CASE(LDPC_Basic)
{
    CSVMatrix* p = new CSVMatrix();
    vector<char> bytes = p->loadCSVFile("data/ldpc/mat1.txt");

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
