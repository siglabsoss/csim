#include "filter_chain.hpp"

FilterChain::FilterChain() :
    m_head(nullptr),
    m_output(),
    m_outputReady(false)
{
}

bool FilterChain::input(const block_io_t &data)
{
    if (m_head == nullptr) {
        return false;
    }

    bool didInput = m_head->input(data);
    tick();
    return didInput;
}

bool FilterChain::output(block_io_t &data)
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
    for (FilterChainElement *current = m_head; current != nullptr; current = current->m_next) {
        //Tick the current element
        current->tick();
        //Check if there's output waiting, move on to tick the next element if no output,
        //otherwise forward the output to the next element
        bool didOutput = current->output(m_output);
        if (didOutput) {
            if (current->m_next == nullptr) {
                m_outputReady = true; //last block in the chain output data
            } else {
                (void)current->m_next->input(m_output);
            }
        }
    }
}

FilterChain & FilterChain::operator=(const FilterChainElement &rhs)
{
    this->m_head = (FilterChainElement *)&rhs;
    return *this;
}
