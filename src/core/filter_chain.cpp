#include <core/filter_chain.hpp>
#include <core/logger.hpp>
#include <utility>

FilterChain::FilterChain() :
    m_head(nullptr),
    m_output(),
    m_outputReady(false),
    m_maxFIFOUtilization(0.0),
    m_didInit(false)
{}

FilterChain::FilterChain(FilterChain && other) :
    m_head(std::move(other.m_head)),
    m_output(other.m_output),
    m_outputReady(other.m_outputReady),
    m_maxFIFOUtilization(other.m_maxFIFOUtilization),
    m_didInit(other.m_didInit)
{}

FilterChain::FilterChain(const FilterChainElement& other) :
    m_head(nullptr),
    m_output(),
    m_outputReady(false),
    m_maxFIFOUtilization(0.0),
    m_didInit(false)
{
    (void)this->operator=(other);
}

void            FilterChain::init()
{
    for (FilterChainElement *current = m_head.get(); current != nullptr;
         current = current->m_next.get()) {
        // Construct a timer for each filter chain element for profiling
        m_timers.push_back(std::unique_ptr<FilterTimer>(new FilterTimer()));
    }
    m_didInit = true;
}

bool FilterChain::input(const filter_io_t& data)
{
    assert(m_didInit);

    if (m_head == nullptr) {
        return false;
    }

    bool didInput = m_head->input(data);
    return didInput;
}

bool FilterChain::output(filter_io_t& data)
{
    if (m_outputReady) {
        data          = m_output;
        m_outputReady = false;
        return true;
    }
    return false;
}

void FilterChain::tick()
{
    double maxFIFOUtilization  = 0;
    size_t elementCounter      = 0;
    size_t maxFIFOElementCount = 0;

    m_timers[elementCounter]->start();

    for (FilterChainElement *current = m_head.get(); current != nullptr;
         current = current->m_next.get()) {
        // Star timer for profiling

        // Tick the current element
        current->tick();

        // Check if there's output waiting, move on to tick the next element if
        // no output,
        // otherwise forward the output to the next element first
        bool didOutput = current->output(m_output);

        m_timers[elementCounter]->stop();

        if (didOutput) {
            if (current->m_next == nullptr) {
                m_outputReady = true; // last block in the chain output data
            } else {
                m_timers[elementCounter + 1]->start();
                bool didInput = current->m_next->input(m_output);

                if (!didInput) {
                    log_err("Filter element %s dropped input sample from %s !",
                            current->m_next->getName().c_str(),
                            current->getName().c_str());
                }
            }
        } else {
            if (current->m_next != nullptr) {
                m_timers[elementCounter + 1]->start();
            }
        }

        if (current->hasInputFIFO()) {
            double utilization = current->inputFIFOUtilization();

            if (utilization > maxFIFOUtilization) {
                maxFIFOUtilization  = utilization;
                maxFIFOElementCount = elementCounter;
            }
        }
        elementCounter++;
    }
    m_maxFIFOUtilization = maxFIFOUtilization;

    if (m_maxFIFOUtilization > 0.7) {
        log_warn(
            "FIFO utilization for FilterChainElement #%d is above threshold! %f",
            maxFIFOElementCount,
            m_maxFIFOUtilization);
    }
}

FilterChain& FilterChain::operator=(const FilterChainElement& rhs)
{
    this->m_head.reset((FilterChainElement *)&rhs);
    return *this;
}

double FilterChain::getFIFOUtilization() const
{
    return m_maxFIFOUtilization;
}

void FilterChain::printTimingReport() const
{
    double total = 0.0;

    for (size_t i = 0; i < m_timers.size(); ++i) {
        total += m_timers[i]->getAverage();
    }

    size_t i = 0;

    for (FilterChainElement *current = m_head.get(); current != nullptr;
         current = current->m_next.get()) {
        std::cout << current->getName() << ": " << m_timers[i]->getAverage() <<
        " " << 100 * m_timers[i]->getAverage() / total << "%" << std::endl;
        i++;
    }
}
