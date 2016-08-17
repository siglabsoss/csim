#include <core/radio_s.hpp>
#include <core/radio_set.hpp>
#include <core/logger.hpp>

#include <mathlib/radio_physics.hpp>

RadioSet::RadioSet() :
    m_radios(),
    m_txBuffers(),
    m_distances(),
    m_noise(3e-11),
    m_didInit(false)
{

}

radio_id_t RadioSet::addRadio(std::function< std::unique_ptr<RadioS>() > &radioFactory)
{
    assert(m_didInit == false); //can't add radios after init (for now)
    std::unique_ptr<RadioS> newRadio = radioFactory();
    RadioS *newRadioPtr = newRadio.get();

    m_radios.push_back(std::move(newRadio));
    size_t numRadios = m_radios.size();

    m_distances.conservativeResize(numRadios, numRadios);
    m_distances(numRadios-1, numRadios-1) = 0; //maintain a diagonal of 0

    for (size_t i = 0; i < numRadios - 1; i++) {
        double distance = (m_radios[i]->getPosition() - newRadioPtr->getPosition()).norm();
        m_distances(numRadios - 1, i) = distance;
        m_distances(i, numRadios - 1) = distance;
    }
    return newRadioPtr->getId();
}

RadioS * RadioSet::getRadioForId(radio_id_t id) const
{
    //XXX make this O(1) instead of O(N)
    for (auto it = m_radios.begin(); it != m_radios.end(); it++) {
        if ((**it).getId() == id) {
            return it->get();
        }
    }
    return nullptr;
}


void RadioSet::init()
{
    assert(m_didInit == false);
    size_t numRadios = m_radios.size();
    log_debug("Initializing radio set with %d radios", numRadios);
    //For each radio, find the other radio with the largest sample delay.
    //This is O(N^2), but it's for init only.
    for (size_t cur = 0; cur < numRadios; cur++) {
        double maxDistance = -1;
        int maxRadioIdx = -1;
        for (size_t i = 0; i < numRadios; i++) {
            double distance = m_distances(cur, i);
            if (distance > maxDistance) {
                maxDistance = distance;
                maxRadioIdx = i;
            }
        }
        //Now we have our max delay to radio 'cur', use it for our ringbuffer size
        int maxDelay = RadioPhysics::sampleDelayForDistance(maxDistance);
        log_debug("Radio %d is farthest away from radio %d with a distance of %f (%d samples)", maxRadioIdx, cur, maxDistance, maxDelay);
        m_txBuffers[m_radios[cur].get()] = CircularBuffer<ComplexDouble >(static_cast<size_t>(maxDelay), ComplexDouble(0.0, 0.0));
    }
    m_didInit = true;
}

void RadioSet::bufferSampleForRadio(const RadioSet::iterator &it, ComplexDouble &sample)
{
    m_txBuffers[(*it).get()].push_front(sample);
}

void RadioSet::getSampleForRadio(const RadioSet::iterator &it, ComplexDouble &sample)
{
    size_t numRadios = m_radios.size();
    sample = ComplexDouble(0.0, 0.0);
    size_t radioOfInterest = std::distance(begin(), it);

    for (size_t i = 0; i < numRadios; i++) {
        if (radioOfInterest == i) { //skip ourself
            continue;
        }
        RadioS *otherRadio = m_radios[i].get();
        double distance = m_distances(i, radioOfInterest);
        int delay = RadioPhysics::sampleDelayForDistance(distance);
        assert (delay <= m_txBuffers[otherRadio].capacity());
        ComplexDouble remoteSample = m_txBuffers[otherRadio].at(delay-1);
        //RadioPhysics::complexRotationForDistance(remoteSample, distance);
        //sample += remoteSample + (m_noise.getNext() * (1.0 / RadioPhysics::freeSpacePowerLoss(distance)));
        sample += remoteSample;
    }
}

RadioSet::iterator RadioSet::begin()
{
    return m_radios.begin();
}

RadioSet::iterator RadioSet::end()
{
    return m_radios.end();
}
