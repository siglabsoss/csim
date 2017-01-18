#include <filters/scrambler_block.hpp>

ScramblerBlock::ScramblerBlock() :
    FilterChainElement("SCRAMBLER"),
    m_scrambler(0b1111111),
    m_didGetInput(false),
    m_output(0)
{}

bool ScramblerBlock::input(const filter_io_t& data)
{
    assert(data.type == IO_TYPE_BIT);
    m_output      = m_scrambler.scramble(data.bit);
    m_didGetInput = true;
    return true;
}

bool ScramblerBlock::output(filter_io_t& data)
{
    if (m_didGetInput) {
        m_didGetInput = false;
        data.type     = IO_TYPE_BIT;
        data.bit      = m_output;
        return true;
    }
    return false;
}

void ScramblerBlock::tick()
{}

void ScramblerBlock::reset(unsigned initState)
{
    m_scrambler.reset(initState);
}
