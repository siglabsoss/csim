#include <core/sigworld.hpp>

SigWorld::SigWorld()
{

}

void SigWorld::addRadio(radio_config_t &config)
{
    m_radioSet.addRadio(config);
}

void SigWorld::init()
{
    m_radioSet.init();
}

void SigWorld::tick()
{
    for (RadioSet::iterator it = m_radioSet.begin(); it != m_radioSet.end(); it++)
    {
        RadioS *current = *it;

        /* Step 1 - Get the current radio's RF sample */
        std::complex<double> sample;
        current->txWave(sample);

        /* Step 2 - Queue the sample in a buffer */
        m_radioSet.bufferSampleForRadio(it, sample);
    }

    for (RadioSet::iterator it = m_radioSet.begin(); it != m_radioSet.end(); it++)
    {
        RadioS *current = *it;

        std::complex<double> sample;
        m_radioSet.getSampleForRadio(it, sample);
    }
}
