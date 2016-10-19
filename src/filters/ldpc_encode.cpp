
#include <filters/ldpc_encode.hpp>

#include <core/parameters.hpp>
#include <core/logger.hpp>

#include <cassert>

static constexpr size_t INPUT_BUFFER_BITS_MAX = 512 * 8;

bool LDPCEncode::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_BYTE);
    //make sure we have enough space left to store the next symbol
    if (INPUT_BUFFER_BITS_MAX - m_inputBuffer.size() < sizeof(data.byte) * 8) {
        return false;
    }
    for (size_t i = 0; i < sizeof(data.byte) * 8; i++) {
        m_inputBuffer.push(data.byte & (1 << i));
    }
    return true;
}

bool LDPCEncode::output(filter_io_t &data)
{

    return true;
}

void LDPCEncode::tick(void)
{
    size_t msgLen = m_G.size();
    if (m_inputBuffer.size() >= msgLen) {
        //we've queued up enough bits
        for (size_t i = 0; i < msgLen; i++) {

        }
    }
}



LDPCEncode::LDPCEncode(const std::vector<std::vector<bool> > &G):
FilterChainElement(std::string("LDPCEncode")),
m_G(G)
{
    assert(m_G.size() > 0 && m_G[0].size() > 0);
    log_debug("Constructed LDPC Encoder for message size of %d and codeword size of %d (rate = %f)", m_G.size(), m_G[0].size(), static_cast<double>(m_G.size()) / static_cast<double>(m_G[0].size()));
}

LDPCEncode::~LDPCEncode()
{

}


std::vector<bool> LDPCEncode::encode(const std::vector<bool> &u) const
{

    size_t msgLen = m_G.size();
    size_t cwLen = m_G[0].size();
    assert(u.size() == msgLen);
    std::vector<bool> cw(cwLen);

    for(size_t i = 0; i < cwLen; i++)
    {
        uint8_t res = 0;
        uint8_t bit;

        for(size_t j = 0; j < msgLen; j++)
        {
            if(m_G[j][i] == 1 && u[j] == 1) {
                bit = 1;
            } else {
                bit = 0;
            }
            res ^= bit;
        }
        cw[i] = res;

    }
    return cw;
}



