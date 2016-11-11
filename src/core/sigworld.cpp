#include <core/sigworld.hpp>
#include <messages/rf_sample.hpp>
#include <vector>

SigWorld::SigWorld() :
    m_radioSet(nullptr),
    m_rxCallback(),
    m_externalSampleSource(),
    m_externalSampleDestination()
{

}

void SigWorld::init(RadioSet *rs)
{
    m_radioSet = rs;
}

void SigWorld::reset()
{
    if (m_radioSet != nullptr) {
        m_radioSet->clear();
    }
}

void SigWorld::tick()
{
    assert(m_radioSet != nullptr);

    for (RadioSet::iterator it = m_radioSet->begin(); it != m_radioSet->end(); it++)
    {
        RadioS * const current = (*it).get();
        ComplexDouble rxSample;
        ComplexDouble txSample;

        /* Step 1 - Feed the radio a waveform sample from the environment */
        if (m_externalSampleSource) {
            m_externalSampleSource(current, rxSample);
        }else {
            m_radioSet->calculateRxSampleForRadio(it, rxSample);
        }

        current->rxWave(rxSample);

        /* Step 2 - trigger the radio's internal processing */
        current->tick();

        /* Step 3 - Get the current radio's RF output */
        current->txWave(txSample);

        /* Step 4 - Queue the sample in a buffer */
        if (m_externalSampleDestination) {
            m_externalSampleDestination(current, txSample);
        } else {
            m_radioSet->bufferTxSampleForRadio(it, txSample);
        }


        /* Step 5 - Check for output bits */
        bool outputBit;
        if (current->rxBit(outputBit)) {
            if (m_rxCallback) {
                m_rxCallback(current->getId(), outputBit);
            } else {
                std::cout << "receive bit " << (int)outputBit << " with no callback to call" << std::endl;
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

bool SigWorld::sendBit(radio_id_t id, bool bit)
{
    return m_radioSet->getRadioForId(id)->txBit(bit);
}

bool SigWorld::sendBits(radio_id_t id, const std::vector<bool> &bits)
{
    bool didSendAll = true;
    //sending with reverse indexing so that LSB is sent first in time
    for (ssize_t i = bits.size() - 1; i >= 0; --i) {
        if (!sendBit(id, bits[i])) {
            didSendAll = false;
            break;
        }
    }
    return didSendAll;
}

void SigWorld::didReceiveBit(std::function< void(radio_id_t, bool) > callback)
{
    m_rxCallback = callback;
}

void SigWorld::setExternalDataSource(std::function< void(const RadioS *current, ComplexDouble &data) > source)
{
    m_externalSampleSource = source;
}

void SigWorld::setExternalDataDestination(std::function< void(const RadioS *current, const ComplexDouble &data) > destination)
{
    m_externalSampleDestination = destination;
}
