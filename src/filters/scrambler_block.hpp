#include <core/filter_chain_element.hpp>
#include <utils/scrambler.hpp>
#include <cassert>

class ScramblerBlock : public FilterChainElement
{
public:
    ScramblerBlock();
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

private:
    Scrambler m_scrambler;
    bool m_didGetInput;
    bool m_output;
};
