#pragma once

#include "block_chain_element.hpp"

class BlockChain : public AbstractSISO < block_io_t, block_io_t >
{

public:
    BlockChain(BlockChainElement *head) :
        m_head(head),
        m_output(),
        m_outputReady(false)
    {
    }

    virtual ~BlockChain() {}

    bool input(const block_io_t &data) override
    {
        if (m_head == nullptr || !m_head->input(data)) {
            return false;
        }

        BlockChainElement *current = m_head;
        do {
            bool didOutput = current->output(m_output);
            current = current->m_next;
            if (didOutput) {
                if (current == nullptr) {
                    m_outputReady = true; //last block in the chain output data
                } else {
                    current->m_next->input(m_output);
                }
            }
        } while (current != nullptr);
        return true;
    }

    bool output(block_io_t &data) override
    {
        if (m_outputReady) {
            data = m_output;
            m_outputReady = false;
            return true;
        }
        return false;
    }
private:
    BlockChainElement *     m_head;
    block_io_t              m_output;
    bool                    m_outputReady;
};
