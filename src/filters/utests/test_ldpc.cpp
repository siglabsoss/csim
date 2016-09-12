#include <test/unit_test.hpp>

#include <filters/ldpc_decode.hpp>
#include <utils/ldpc_utils.hpp>

#include <boost/tokenizer.hpp> //For parsing data from file

CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

//CSIM_TEST_CASE(LDPC_Basic)
//{
//    cout << "hi" << endl;
//    LDPCDecode decode;
//    decode.run();
//}

CSIM_TEST_CASE(CSV_Parse)
{
    CSVMatrix* p = new CSVMatrix();

    vector<char> bytes;
    bytes = p->loadCSVFile("data/ldpc/mat1.txt");

    uint32_t rows, cols;

    vector<vector<uint8_t> > H;

    p->parseCSV(rows, cols, bytes, H);

    cout << "H rows " << H.size() << " H cols " << H[0].size() << endl;

    cout << endl << endl;

    for (auto it = H.begin(); it != H.end(); ++it) {
        for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
            cout << (int) *it2 << ",";
        }
        cout << endl;
    }
}

CSIM_TEST_SUITE_END()
