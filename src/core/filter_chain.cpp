#include <core/filter_chain.hpp>


FilterChain::FilterChain() :
    m_head(nullptr),
    m_output(),
    m_outputReady(false)
{
}


FilterChain::FilterChain(const FilterChain &other) :
        m_head(other.m_head),
        m_output(other.m_output),
        m_outputReady(other.m_outputReady)
{

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
    for (FilterChainElement *current = m_head; current != nullptr; current = current->m_next) {
        //Tick the current element
        current->tick();
        //Check if there's output waiting, move on to tick the next element if no output,
        //otherwise forward the output to the next element first
        bool didOutput = current->output(m_output);
        if (didOutput) {
            if (current->m_next == nullptr) {
                m_outputReady = true; //last block in the chain output data
            } else {
                (void)current->m_next->input(m_output);
            }
            //Publish the output externally
            publish(current);
        }
    }
}

FilterChain & FilterChain::operator=(const FilterChainElement &rhs)
{
    this->m_head = (FilterChainElement *)&rhs;
    return *this;
}

void FilterChain::publish(FilterChainElement *current)
{
    uint8_t buf[200] = {0};
    uint8_t *head = buf;

    memcpy(head, current->getName().c_str(), current->getName().size());
    head += current->getName().size();

    size_t size = m_output.serialize(head);

    Publisher::get()->send("FILT", buf, size + current->getName().size());
}
