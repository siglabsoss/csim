#pragma once

#include <core/radio_s.hpp>
#include <mathlib/complex_gaussian_noise.hpp>

#include <vector>
#include <map>
#include <Eigen/Dense>
#include <boost/circular_buffer.hpp>

using Eigen::MatrixXd;



class RadioSet
{
public:
    typedef std::vector<std::unique_ptr<RadioS> >::iterator iterator;

public:
    RadioSet();
    /**
     * addRadio - Add a new radio to the set of radios.
     * @param config The new radios configuration.
     * @return ID of radio
     */
    radio_id_t addRadio(std::function< std::unique_ptr<RadioS>() > &radioFactory);

    void init();

    void bufferSampleForRadio(const iterator &it, std::complex<double> &sample);
    void getSampleForRadio(const iterator &it, std::complex<double> &sample);

    iterator begin();
    iterator end();

private:
    std::vector<std::unique_ptr<RadioS> >                               m_radios;
    std::map<RadioS *, boost::circular_buffer<std::complex<double> > >  m_txBuffers;
    MatrixXd                                                            m_distances;
    ComplexGaussianNoise                                                m_noise;
    bool                                                                m_didInit;
};
