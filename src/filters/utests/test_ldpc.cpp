#include <test/unit_test.hpp>

#include <filters/ldpc_decode.hpp>



CSIM_TEST_SUITE_BEGIN(LDPCFunctionality)

CSIM_TEST_CASE(LDPC_Basic)
{
    cout << "hi" << endl;
    LDPCDecode decode;
    decode.run();

}



CSIM_TEST_SUITE_END()
