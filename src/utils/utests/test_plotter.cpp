#include <test/unit_test.hpp>

#include <utils/utils.hpp>
#include <utils/plotter.hpp>

CSIM_TEST_SUITE_BEGIN(PlotterUtilitiesTests)

CSIM_TEST_CASE(BASIC)
{
    using namespace std;

    const plotter &plot = plotter::get();

    CircularBuffer<int> ivector(100);
    for (int i = 0; i < 100; i++)
    {
        ivector.push_back(i);
    }

//    cout << ivector << endl;

    plot.nplot(ivector, "ramp");


}

CSIM_TEST_SUITE_END()
