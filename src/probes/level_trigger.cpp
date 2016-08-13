#include <probes/level_trigger.hpp>

LevelTrigger::LevelTrigger(const std::string &name, size_t sampleCount, double level) :
    FilterProbe(name, sampleCount),
    m_level(abs(level)),
    m_didTrigger(false)
{

}

LevelTrigger::~LevelTrigger()
{

}

void LevelTrigger::tick(void)
{
    if (m_validInput) {
        double mag = std::abs(getLatest().toComplexDouble());
        if (!m_didTrigger && didTrigger(mag)) {
            m_didTrigger = true;
            std::ostringstream title;
            title << mag << " > " << m_level;
            trigger(title.str());
        }
    }
}

bool LevelTrigger::didTrigger(double level) const
{
    if (level > m_level) {
        return true;
    }
    return false;
}
