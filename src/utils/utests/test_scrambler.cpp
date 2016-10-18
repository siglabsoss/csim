#include <test/unit_test.hpp>

#include <utils/scrambler.hpp>

CSIM_TEST_SUITE_BEGIN(ScramblerValidation)

CSIM_TEST_CASE(BIT_SEQUENCE)
{
    std::vector<bool> dummyData(254, false);
    std::vector<bool> initState(7, true);
    Scrambler s(initState);
    s.scramble(dummyData);
}


CSIM_TEST_SUITE_END()
