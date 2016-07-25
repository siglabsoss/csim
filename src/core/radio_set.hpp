#pragma once

#include <core/radio_s.hpp>

#include <vector>
#include <map>
#include <Eigen/Dense>
#include <boost/circular_buffer.hpp>

using Eigen::MatrixXd;



class RadioSet
{
public:
    typedef std::vector<RadioS *>::iterator iterator;

public:
    RadioSet();
    /**
     * addRadio - Add a new radio to the set of radios.
     * @param config The new radios configuration.
     */
    void addRadio(RadioS *(radioFactory)(const radio_config_t &config), radio_config_t &config);
    void init();
    void bufferSampleForRadio(const iterator &it, std::complex<double> &sample);

    void getSampleForRadio(const iterator &it, std::complex<double> &sample);

    /**
     * getLargestSampleDelay - returns the largest delay between any two radios from
     * the set of radios added. This can be useful in cases where you want to set up
     * upper bounds on some kind of buffer to store delayed transmissions.
     *
     * @return The largest sample delay in number of "ticks"
     */
    int getLargestSampleDelay();

    iterator begin();
    iterator end();

private: //methods
    static int sampleDelayForDistance(double distance);

private:
    std::vector<RadioS *> m_radios;
    std::map<RadioS *, boost::circular_buffer<std::complex<double> > > m_txBuffers;
    MatrixXd    m_distances;

    bool        m_didInit;
};
