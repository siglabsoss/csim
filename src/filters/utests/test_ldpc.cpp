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
    cout << "hi2" << endl;
    CSVMatrix* p = new CSVMatrix();



   ifstream ifs("data/ldpc/mat1.txt", ios::binary|ios::ate);
   ifstream::pos_type bytecount = ifs.tellg();

   cout << "pos " << bytecount << endl;

   std::vector<char>  result(bytecount);
   ifs.seekg(0, ios::beg);
   ifs.read(&result[0], bytecount);

   uint32_t rows, cols;

   vector<vector<uint8_t> > H;

   p->parseSize(&rows, &cols, result, H);


   cout << "H rows " << H.size() << " H cols " << H[0].size() << endl;

   cout << endl << endl;



   for(auto it = H.begin(); it != H.end(); ++it)
   {
       for(auto it2 = it->begin(); it2 != it->end(); ++it2)
       {
           cout << (int)*it2 << ",";
       }
//       cout << *it;
       cout << endl;
   }
//
//
//   cout << endl;



//   return result;



}



CSIM_TEST_SUITE_END()
