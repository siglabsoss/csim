#include <test/unit_test.hpp>

#include <channel/pathloss.hpp>

CSIM_TEST_SUITE_BEGIN(TestSuiteNameHere)

void runPathLossTests(const std::vector< std::pair <double, double> > &tests, PathLossModel::model_t model)
{
    static constexpr double fc = 2.0e9;
    static constexpr double htx = 10.0;
    static constexpr double hrx = 2.0;
    PathLossModel::terrain_t terrain = PathLossModel::TERRAIN_A;

    PathLossModel plm(terrain, model, fc, htx, hrx);

    for (size_t i = 0; i < tests.size(); i++) {
        double expected = tests[i].second;
        double actual = plm.getPathLossForDistance(tests[i].first);
        BOOST_CHECK_CLOSE(actual, expected, 0.002);
    }
}

CSIM_TEST_CASE(ATT_PATHLOSS_CALCULATION)
{
    PathLossModel::model_t model     = PathLossModel::ATT;

    std::vector< std::pair    <double, double> > tests;
    tests.push_back( std::pair<double, double>(10.0, 58.4624) );
    tests.push_back( std::pair<double, double>(20.0, 64.4830) );
    tests.push_back( std::pair<double, double>(1234.0, 141.5950) );
    tests.push_back( std::pair<double, double>(0.01, -1.5376) );
    tests.push_back( std::pair<double, double>(20000.0, 211.5770) );

    runPathLossTests(tests, model);
}

CSIM_TEST_CASE(OKUMURA_PATHLOSS_CALCULATION)
{
    PathLossModel::model_t model     = PathLossModel::OKUMURA;

    std::vector< std::pair    <double, double> > tests;
    tests.push_back( std::pair<double, double>(10.0, 58.4624) );
    tests.push_back( std::pair<double, double>(20.0, 64.4830) );
    tests.push_back( std::pair<double, double>(1234.0, 142.7471) );
    tests.push_back( std::pair<double, double>(20000.0, 212.7291) );

    runPathLossTests(tests, model);
}

CSIM_TEST_SUITE_END()
