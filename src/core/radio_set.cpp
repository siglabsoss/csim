#include <core/radio_s.hpp>
#include <core/radio_set.hpp>
#include <core/logger.hpp>

RadioSet::RadioSet() :
    m_radios(),
    m_txBuffers(),
    m_distances(),
    m_didInit(false)
{

}

void RadioSet::addRadio(radio_config_t &config)
{
    assert(m_didInit == false); //can't add radios after init (for now)
    RadioS *newRadio = RadioS::create(config);

    assert(newRadio != nullptr);

    m_radios.push_back(newRadio);
    size_t numRadios = m_radios.size();

    m_distances.conservativeResize(numRadios, numRadios);
    m_distances(numRadios-1, numRadios-1) = 0; //maintain a diagonal of 0

    for (size_t i = 0; i < numRadios - 1; i++) {
        double distance = (m_radios[i]->getPosition() - newRadio->getPosition()).norm();
        m_distances(numRadios - 1, i) = distance;
        m_distances(i, numRadios - 1) = distance;
    }
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
        int maxDelay = sampleDelayForDistance(maxDistance);
        log_debug("Radio %d is farthest away from radio %d with a distance of %f (%d samples)", maxRadioIdx, cur, maxDistance, maxDelay);
        m_txBuffers[m_radios[cur]] = boost::circular_buffer<std::complex<double> >(static_cast<size_t>(maxDelay), std::complex<double>(0.0, 0.0));
    }
    m_didInit = true;
}

int RadioSet::sampleDelayForDistance(double distance)
{
    constexpr double METERS_PER_TICK = 1.19916983; //TODO make configurable
    return static_cast<uint32_t>(distance / METERS_PER_TICK);
}

void RadioSet::bufferSampleForRadio(const RadioSet::iterator &it, std::complex<double> &sample)
{
    m_txBuffers[*it].push_back(sample);
}

void RadioSet::getSampleForRadio(const RadioSet::iterator &it, std::complex<double> &sample)
{
    size_t numRadios = m_radios.size();
    sample = std::complex<double>(0.0, 0.0);
    size_t radioOfInterest = std::distance(begin(), it);

    for (size_t i = 0; i < numRadios; i++) {
        if (radioOfInterest == i) { //skip ourself
            continue;
        }
        RadioS *radio = m_radios[i];
        double distance = m_distances(i, radioOfInterest);
        int delay = sampleDelayForDistance(distance);
        assert (delay <= m_txBuffers[radio].capacity());
        sample += m_txBuffers[radio].at(delay); //TODO power loss
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
