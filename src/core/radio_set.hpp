#pragma once

#include <core/radio_s.hpp>
#include <mathlib/complex_gaussian_noise.hpp>
#include <types/circularbuffer.hpp>

#include <vector>
#include <map>
#include <Eigen/Dense>

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
    radio_id_t addRadio(std::function< std::unique_ptr<RadioS>() > radioFactory);
    RadioS *getRadioForId(radio_id_t id) const;

    void init(bool noNoise, bool noDelay, bool noPhaseRot);
    void clear();

    void bufferTxSampleForRadio(const iterator &it, ComplexDouble &sample);
    void bufferTxSampleForRadio(radio_id_t id, ComplexDouble &sample);

    void calculateRxSampleForRadio(const iterator &it, ComplexDouble &sample);
    void calculateRxSampleForRadio(radio_id_t id, ComplexDouble &sample);

    iterator begin();
    iterator end();

private:
    void bufferTxSampleForRadio(const RadioS *radio, ComplexDouble &sample);
    void calculateRxSampleForRadio(const RadioS *radio, ComplexDouble &sample);

private:
    std::vector<std::unique_ptr<RadioS> >                               m_radios;
    std::map<const RadioS *, CircularBuffer<ComplexDouble > >           m_txBuffers;
    MatrixXd                                                            m_distances;
    ComplexGaussianNoise                                                m_noise;
    bool                                                                m_didInit;

    //Configuration options
    bool                                                                m_noNoise;
    bool                                                                m_noDelay;
    bool                                                                m_noPhaseRot;
};
