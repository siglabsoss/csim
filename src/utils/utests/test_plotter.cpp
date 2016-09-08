#include <test/unit_test.hpp>

#include <utils/utils.hpp>
#include <utils/plotter.hpp>


CSIM_TEST_SUITE_BEGIN(PlotterUtilitiesTests)


// this test calls the python plotter, but no assertions are made that the data looks correct
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

// this test calls the python plotter, but no assertions are made that the data looks correct
CSIM_TEST_CASE(BER_COOKED)
{
    using namespace std;

    const plotter &plot = plotter::get();

    vector<double> ber1 {0.158368318809598, 0.130644488522829, 0.103759095953406, 0.0786496035251426, 0.0562819519765415, 0.037506128358926, 0.0228784075610853, 0.0125008180407376, 0.00595386714777866, 0.00238829078093281, 0.000772674815378444, 0.000190907774075993, 3.36272284196175e-05, 3.87210821552204e-06};
    vector<double> ebn1 {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    string title1("theoretical");

    vector<double> ber2 {0.144, 0.116, 0.098, 0.076, 0.058, 0.035, 0.021, 0.009, 0.004, 0.002, 0.002, 0, 0, 0};
    vector<double> ebn2 {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    string title2("sim");

    vector<vector<double> > bers;
    vector<vector<double> > ebnos;
    vector<string> titles;


    bers.push_back(ber1);
    ebnos.push_back(ebn1);
    titles.push_back(title1);

    bers.push_back(ber2);
    ebnos.push_back(ebn2);
    titles.push_back(title2);

    plot.nplotber(bers, ebnos, titles);

}

CSIM_TEST_SUITE_END()
