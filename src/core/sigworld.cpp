#include <core/sigworld.hpp>
#include <messages/rf_sample.hpp>

SigWorld::SigWorld() :
    m_radioSet()
{

}

void SigWorld::init(bool noNoise, bool noDelay, bool noPhaseRot)
{
    m_radioSet.init(noNoise, noDelay, noPhaseRot);
}

void SigWorld::reset()
{
    m_radioSet.clear();
}

void SigWorld::tick()
{
    for (RadioSet::iterator it = m_radioSet.begin(); it != m_radioSet.end(); it++)
    {
        RadioS * const current = (*it).get();
        ComplexDouble rxSample;
        ComplexDouble txSample;

        /* Step 1 - Feed the radio a waveform sample from the environment */
        m_radioSet.getSampleForRadio(it, rxSample);
        current->rxWave(rxSample);

        /* Step 2 - trigger the radio's internal processing */
        current->tick();

        /* Step 3 - Get the current radio's RF output */
        current->txWave(txSample);

        /* Step 4 - Queue the sample in a buffer */
        m_radioSet.bufferSampleForRadio(it, txSample);

        /* Step 5 - Check for output bytes */
        uint8_t outputByte;
        if (current->rxByte(outputByte)) {
            if (m_rxCallback) {
                m_rxCallback(current->getId(), outputByte);
            } else {
                std::cout << "receive " << (int)outputByte << " with no callback to call" << std::endl;
            }
        }

        /* Step 6 - Publish data */
//        RFSample sample;
//        sample.id     = (uint64_t)current->getId();
//        sample.rxReal = rxSample.real();
//        sample.rxImag = rxSample.imag();
//        sample.txReal = txSample.real();
//        sample.txImag = txSample.imag();
//        Publisher::get()->send("RF", (uint8_t *)(&sample), sizeof(sample));

    }
}

void SigWorld::addRadio(std::function< std::unique_ptr<RadioS>() > radioFactory)
{
    (void)m_radioSet.addRadio(radioFactory);
}

bool SigWorld::sendByte(radio_id_t id, uint8_t byte)
{
    return m_radioSet.getRadioForId(id)->txByte(byte);
}

void SigWorld::didReceiveByte(std::function< void(radio_id_t, uint8_t) > callback)
{
    m_rxCallback = callback;
}
