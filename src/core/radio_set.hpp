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
     * @return ID of radio
     */
    radio_id_t addRadio(RadioS *(radioFactory)(const radio_config_t &config), radio_config_t &config);

    void init();

    void bufferSampleForRadio(const iterator &it, std::complex<double> &sample);
    void getSampleForRadio(const iterator &it, std::complex<double> &sample);

    iterator begin();
    iterator end();

private:
    std::vector<RadioS *> m_radios;
    std::map<RadioS *, boost::circular_buffer<std::complex<double> > > m_txBuffers;
    MatrixXd    m_distances;

    bool        m_didInit;
};
