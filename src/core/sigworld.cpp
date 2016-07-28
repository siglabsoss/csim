#include <core/sigworld.hpp>
#include <messages/rf_sample.hpp>

SigWorld::SigWorld() :
    m_radioSet()
{

}

void SigWorld::addRadio(std::unique_ptr<RadioS> (radioFactory)(const radio_config_t &config), radio_config_t &config)
{
    (void)m_radioSet.addRadio(radioFactory, config);
}

void SigWorld::init()
{
    m_radioSet.init();
}

void SigWorld::tick()
{
    size_t count = 0;
    for (RadioSet::iterator it = m_radioSet.begin(); it != m_radioSet.end(); it++)
    {
        RadioS * const current = (*it).get();
        RFSample sample; //for publishing externally
        std::complex<double> rxSample;
        std::complex<double> txSample;

        /* Step 1 - Feed the radio a waveform sample from the environment */
        m_radioSet.getSampleForRadio(it, rxSample);
        current->rxWave(rxSample);

        /* Step 2 - trigger the radio's internal processing */
        current->tick();

        /* Step 3 - Get the current radio's RF output */
        current->txWave(txSample);

        /* Step 4 - Queue the sample in a buffer */
        m_radioSet.bufferSampleForRadio(it, txSample);

        /* Step 5 - Publish data */
        sample.id     = (uint64_t)current->getId();
        sample.rxReal = rxSample.real();
        sample.rxImag = rxSample.imag();
        sample.txReal = txSample.real();
        sample.txImag = txSample.imag();
        Publisher::get()->send("RF", (uint8_t *)(&sample), sizeof(sample));

        count++;
    }
}
