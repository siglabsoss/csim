#include <core/filter_chain_element.hpp>
#include <utils/scrambler.hpp>
#include <cassert>

class ScramblerBlock : public FilterChainElement
{
public:

    ScramblerBlock();
    bool input(const filter_io_t& data) override;
    bool output(filter_io_t& data) override;
    void tick(void) override;

    void reset(unsigned initState);

private:

    Scrambler m_scrambler;
    bool m_didGetInput;
    bool m_output;
};

std::vector<bool>ScramblerWrap(const std::vector<bool>& inputs);
