#include <core/filter_chain.hpp>
#include <core/logger.hpp>
#include <utility>

FilterChain::FilterChain() :
    m_head(nullptr),
    m_output(),
    m_outputReady(false),
    m_maxFIFOUtilization(0.0)
{
}


FilterChain::FilterChain(FilterChain &&other) :
        m_head(std::move(other.m_head)),
        m_output(other.m_output),
        m_outputReady(other.m_outputReady),
        m_maxFIFOUtilization(other.m_maxFIFOUtilization)
{
}

FilterChain::FilterChain(const FilterChainElement &other) :
        m_head(nullptr),
        m_output(),
        m_outputReady(false),
        m_maxFIFOUtilization(0.0)
{
    (void)this->operator=(other);
}

void FilterChain::init()
{

}

bool FilterChain::input(const filter_io_t &data)
{
    if (m_head == nullptr) {
        return false;
    }

    bool didInput = m_head->input(data);
    return didInput;
}

bool FilterChain::output(filter_io_t &data)
{
    if (m_outputReady) {
        data = m_output;
        m_outputReady = false;
        return true;
    }
    return false;
}

void FilterChain::tick()
{
    double maxFIFOUtilization = 0;
    for (FilterChainElement * current = m_head.get(); current != nullptr; current = current->m_next.get()) {
        //Tick the current element
        current->tick();
        //Check if there's output waiting, move on to tick the next element if no output,
        //otherwise forward the output to the next element first
        bool didOutput = current->output(m_output);
        if (didOutput) {
            if (current->m_next == nullptr) {
                m_outputReady = true; //last block in the chain output data
            } else {
                bool didInput = current->m_next->input(m_output);
                if (!didInput) {
                    log_err("Filter element %s dropped input sample from %s !", current->m_next->getName().c_str(), current->getName().c_str());
                }
            }
        }
        if (current->hasInputFIFO()) {
            double utilization = current->inputFIFOUtilization();
            if (utilization > maxFIFOUtilization) {
                maxFIFOUtilization = utilization;
            }
        }
    }
    m_maxFIFOUtilization = maxFIFOUtilization;
    if (m_maxFIFOUtilization > 0.7) {
        log_warn("FIFO utilization is above threshold! %f", m_maxFIFOUtilization);
    }
}

FilterChain & FilterChain::operator=(const FilterChainElement &rhs)
{
    this->m_head.reset((FilterChainElement *)&rhs);
    return *this;
}

double FilterChain::getFIFOUtilization() const
{
    return m_maxFIFOUtilization;
}
