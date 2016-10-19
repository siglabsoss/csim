
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
    //we go from "left to right" taking the most significant bit first
    for (ssize_t i = 7; i >= 0 ; i--) {
        bool bit = data.byte & (1 << i);
        m_inputBuffer.push(bit);
    }
    return true;
}

bool LDPCEncode::output(filter_io_t &data)
{
    if (m_outputBuffer.size() >= 8) {
        uint8_t byte = 0;
        //we go from "left to right" taking the most significant bit first
        for (ssize_t i = 7; i >= 0 ; i--) {
            uint8_t nextBit = !!m_outputBuffer.front();
            m_outputBuffer.pop();
            byte |= (nextBit << i);
        }
        data.type = IO_TYPE_BYTE;
        data.byte = byte;
        return true;
    }
    return false;
}

void LDPCEncode::tick(void)
{
    size_t msgLen = m_G.size();
    if (m_inputBuffer.size() >= msgLen) {
        //we've queued up enough bits
        std::vector<bool> msg(msgLen);
        for (size_t i = 0; i < msgLen; i++) { //XXX do we loop from start to end or vice versa?
            bool nextBit = !!m_inputBuffer.front();
            m_inputBuffer.pop();
            msg[i] = nextBit;
        }
        std::vector<bool> cw = encode(msg);
        for (size_t i = 0; i < cw.size(); i++) {
            m_outputBuffer.push(cw[i]);
        }
    }
}

LDPCEncode::LDPCEncode(const std::vector<std::vector<bool> > &G):
    FilterChainElement(std::string("LDPCEncode")),
    m_G(G),
    m_inputBuffer(),
    m_outputBuffer()
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

        for(size_t j = 0; j < msgLen; j++)
        {
            res ^= (m_G[j][i] == 1 && u[j] == 1);
        }
        cw[i] = res;

    }
    return cw;
}



