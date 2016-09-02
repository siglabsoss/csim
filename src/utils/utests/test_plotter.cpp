#include <test/unit_test.hpp>

#include <utils/utils.hpp>
#include <utils/plotter.hpp>

CSIM_TEST_SUITE_BEGIN(PlotterUtilitiesTests)

CSIM_TEST_CASE(BASIC)
{
    using namespace std;

    const plotter &plot = plotter::get();

    vector<int> ivector;
    vector<complex<double> > cdv;
    CircularBuffer<complex<double> > cd(100);
    CircularBuffer<int> ci(100);


    for (int i = 0; i < 100; i++)
    {
        ivector.push_back(i);
        cdv.push_back(complex<double>(i,i*2));
        cd.push_back(complex<double>(i,i*2));
        ci.push_back(i);

    }







    plot.nplot(ivector, "ramp");
    plot.nplot(cdv, "complexdouble vector");
    plot.nplot(cd, "complexfloat");
    plot.nplot(ci, "cb int");



}

CSIM_TEST_SUITE_END()
