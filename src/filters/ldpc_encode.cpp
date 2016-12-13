
#include <filters/ldpc_encode.hpp>

#include <core/parameters.hpp>
#include <core/logger.hpp>

#include <cassert>

static constexpr size_t INPUT_BUFFER_BITS_MAX = 512 * 8;

bool LDPCEncode::output(filter_io_t &data)
{
    if (m_outputBuffer.size() > 0) {
        bool nextBit = !!m_outputBuffer.front(); //we're popping out LSB first
        m_outputBuffer.pop();
        data.type = IO_TYPE_BIT;
        data.bit = nextBit;
        //std::cout << "enc out: " << (int)nextBit << std::endl;
        return true;
    }
    return false;
}

void LDPCEncode::tick(void)
{
    size_t msgLen = m_G.size();
    if (m_fifo.size() >= msgLen) {
        //we've queued up enough bits
        std::vector<bool> msg(msgLen);
        //we're popping off LSB first and storing MSB in msg[0]
        for (ssize_t i = msgLen - 1; i >= 0; --i) {
            filter_io_t sample = m_fifo.front();
            assert(sample.type == IO_TYPE_BIT);
            bool nextBit = sample.bit;
            m_fifo.pop_front();
            msg[i] = nextBit;
        }
//        std::cout << "msg: ";
//        for (size_t i = 0; i < msgLen; i++) {
//            std::cout << (int)msg[i] << " ";
//        }
//        std::cout << std::endl;
        std::vector<bool> cw = encode(msg);
        for (ssize_t i = cw.size() - 1; i >= 0; --i) {
            //we're pushing LSB onto the FIFO first so that we output LSB first
            m_outputBuffer.push(cw[i]);
        }
    }
}

size_t LDPCEncode::getMsgLen() const
{
    return m_G.size();
}

size_t LDPCEncode::getCwLen() const
{
    return m_G[0].size();
}

LDPCEncode::LDPCEncode(const std::vector<std::vector<bool> > &G):
    FilterChainElement(std::string("LDPCEncode"), INPUT_BUFFER_BITS_MAX),
    m_G(G),
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



