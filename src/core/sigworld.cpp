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

        /* Step 1 - Feed the radio a waveform sample from the environment */
        std::complex<double> rxSample;
        m_radioSet.getSampleForRadio(it, rxSample);
        current->rxWave(rxSample);

        /* Step 2 - trigger the radio's internal processing */
        current->tick();

        /* Step 3 - Get the current radio's RF output */
        std::complex<double> txSample;
        current->txWave(txSample);

        /* Step 4 - Queue the sample in a buffer */
        m_radioSet.bufferSampleForRadio(it, txSample);
    }
}
