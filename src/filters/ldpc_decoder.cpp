
#include <filters/ldpc_decoder.hpp>

#include <core/parameters.hpp>

#include <cassert>

LDPCDecoder::LDPCDecoder(const std::vector<std::vector<bool> > &H):
FilterChainElement(std::string("LDPCDecoder")),
m_hrows(H.size()),
m_hcols(H[0].size()), //will crash if hrows == 0
m_H(H),
m_codeBits(m_hcols),
m_checkNodes(m_hrows),
m_messages(),
m_softInputBits(),
m_hardOutputBits()
{
    parseH();
}

LDPCDecoder::~LDPCDecoder()
{

}

bool LDPCDecoder::input(const filter_io_t &data)
{
    assert(data.type == IO_TYPE_FIXPOINT);
    //Receiving LSB first
    m_softInputBits.push(data.fp);
    return true;
}

bool LDPCDecoder::output(filter_io_t &data)
{
    if (m_hardOutputBits.size() > 0) {
        bool bit = m_hardOutputBits.front();
        m_hardOutputBits.pop();
        data.type = IO_TYPE_BIT;
        data.bit = bit;
        return true;
    }
    return false;
}

void LDPCDecoder::tick(void)
{
    if (m_softInputBits.size() >= m_hcols) { //we have enough bits to represent a codeword
        assert(m_hardOutputBits.size() == 0);
        std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > cw(m_hcols);
        for (ssize_t i = m_hcols - 1; i >= 0; --i) {
            cw[i] = m_softInputBits.front(); //LSBs are popped off first
            m_softInputBits.pop();
        }
        assert(cw.size() == m_codeBits.size());
        assert(m_codeBits.size() == m_hcols);

        for (size_t i = 0; i < m_codeBits.size(); i++) {
            m_codeBits[i].softChannelEstimate = cw[i];
        }

        bool didSolve = false;
        size_t solvedIterationNumber = 0;
        decode(20, didSolve, solvedIterationNumber);

        if (didSolve) {
            if (solvedIterationNumber != 0) {
                std::cout << "LDPC decode success after " << solvedIterationNumber << " iterations" << std::endl;
            }
        } else {
            std::cout << "LDPC decode failed" << std::endl;
            std::cout << "Before Decode: ";
            for (size_t i = 0; i < cw.size(); ++i) {
                std::cout << LLRToBit(cw[i].to_double());
                if (i != cw.size() - 1) {
                    std::cout << ",";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << "After Decode: ";
            for (size_t i = 0; i < m_codeBits.size(); ++i) {
                std::cout << LLRToBit(m_codeBits[i].LLR.to_double());
                if (i != m_codeBits.size() - 1) {
                    std::cout << ",";
                } else {
                    std::cout << std::endl;
                }
            }
        }

        //Message size is equal to rows for 80211n codes and the msg is sitting in the first part of the codeword (XXX this is not a generic property!)
        size_t msgLength = m_hrows;
        for (size_t i = 0; i < msgLength; i++) {
            bool bit = LLRToBit(m_codeBits[msgLength - 1 - i].LLR.to_double());
            m_hardOutputBits.push(bit);
        }
        resetDecoderState();
    }
}

void LDPCDecoder::decode(size_t iterations, bool& solved, size_t& solved_iterations)
{
    // These stay here if code is unsolved
    solved = false;
    solved_iterations = 0;

    for(size_t i = 0; i < iterations; ++i)
    {
        updateLLR();
        bool justSolved = (parityCheck()== 0);
        if (justSolved && !solved) {
            solved = true;
            solved_iterations = i;
            return; //in hardware we may always complete worst-case iterations, but we'll exit early in software to speed up simulation
        }
        iteration();
    }
}

void LDPCDecoder::resetDecoderState()
{
    //reset all memories related to decoding (this excludes IO buffers)
    for (size_t i = 0; i < m_codeBits.size(); ++i) {
        m_codeBits[i].LLR = 0.0;
        m_codeBits[i].softChannelEstimate = 0.0;
    }

    for (auto it = m_messages.begin(); it != m_messages.end(); ++it) {
        it->second = 0.0;
    }

    for (auto it = m_tmpMsgs.begin(); it != m_tmpMsgs.end(); ++it) {
        it->second = 0.0;
    }
}

void LDPCDecoder::parseH()
{
    //Each row represents a parity check node, which in turn represents a parity check equation.
    //Each column represents a code bit
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        for( unsigned j = 0; j < m_hcols; ++j)
        {
            if( m_H[i][j] )
            {
                //Build adjacency lists for both check and bit nodes involved in each connection
                CheckNode *check = &m_checkNodes[i];
                BitNode *bit = &m_codeBits[j];
                check->checkNum = i;
                bit->bitNum = j;
                check->bits.push_back(bit);
                bit->checks.push_back(check);

                //Store an LLR value for each edge connection to represent the bit-to-check and check-to-bit messages
                GraphEdgeKey key(i, j);
//                std::cout << "LDPC graph edge (" << i << "," << j << ")" << std::endl;
                m_messages[key] = 0.0;
                m_tmpMsgs[key] = 0.0;
            }
        }

//        std::cout << "Check node " << i << " has degree " << m_checkNodes[i].bits.size() << std::endl;
    }
}

void LDPCDecoder::iteration()
{
    //Bits-to-checks pass: Create messages for each edge by taking the bit's LLR given by the channel, summing
    //it with the estimates from the previous checks-to-bits pass, excluding the information that was given from the target node
    for (size_t bit = 0; bit < m_codeBits.size(); ++bit) {
        SLFixedPoint<LDPC_LLR_FORMAT> llr = m_codeBits[bit].softChannelEstimate;
        for (size_t i = 0; i < m_codeBits[bit].checks.size(); ++i) {
            CheckNode *currentCheckNode = m_codeBits[bit].checks[i];
            //std::cout << m_messages[key] << std::endl;
//            std::cout << llr.to_double() << " + " << m_messages[key].to_double();
            llr = llr.to_double() + m_messages[GraphEdgeKey(currentCheckNode->checkNum, bit)].to_double();
//            std::cout << " = " << llr.to_double() << std::endl;
//            std::cout << "Bits to Checks (" << key.bitNum << "," << key.checkNum << ") " << m_messages[key].to_double() << " -> " << llr.to_double() << std::endl;

        }
        for (size_t i = 0; i < m_codeBits[bit].checks.size(); ++i) {
            CheckNode *currentCheckNode = m_codeBits[bit].checks[i];
            m_tmpMsgs[GraphEdgeKey(currentCheckNode->checkNum, bit)] = llr.to_double() - m_messages[GraphEdgeKey(currentCheckNode->checkNum, bit)].to_double();
        }
    }
    m_messages = m_tmpMsgs;
    //Checks-to-bits pass: Create messages for each edge by taking the minimum...
    for (size_t check = 0; check < m_checkNodes.size(); ++check) {
        double minMag = static_cast<double>(1ull << LDPC_LLR_IWL); //starting with larger magnitude than possible
        double secondMinMag = static_cast<double>(1ull << LDPC_LLR_IWL); //starting with larger magnitude than possible
        double sign = 1.0;
        for (size_t i = 0; i < m_checkNodes[check].bits.size(); ++i) {
            BitNode *currentBit = m_checkNodes[check].bits[i];

            double msgVal = m_messages[GraphEdgeKey(check, currentBit->bitNum)].to_double();
            if (std::abs(msgVal) < minMag) {
                minMag = std::abs(msgVal);
            } else if (std::abs(msgVal) < secondMinMag) {
                secondMinMag = std::abs(msgVal);
            }
            sign *= (msgVal < 0.0) ? -1.0 : 1.0;
        }
        for (size_t i = 0; i < m_checkNodes[check].bits.size(); ++i) {
            BitNode *currentBit = m_checkNodes[check].bits[i];
            double msgVal = m_messages[GraphEdgeKey(check, currentBit->bitNum)].to_double();
            double finalSign = sign;
            finalSign *= (msgVal < 0.0) ? -1.0 : 1.0;
            if (std::abs(msgVal) == minMag) {
                SLFixedPoint<LDPC_LLR_FORMAT> val(finalSign * secondMinMag);
                m_tmpMsgs[GraphEdgeKey(check, currentBit->bitNum)] = val;
            } else {
                SLFixedPoint<LDPC_LLR_FORMAT> val(finalSign * minMag);
                m_tmpMsgs[GraphEdgeKey(check, currentBit->bitNum)] = val;
            }
        }
    }
    m_messages = m_tmpMsgs;
}

size_t LDPCDecoder::parityCheck() const
{
    size_t result = 0;
    for (size_t i = 0; i < m_checkNodes.size(); i++) {
        size_t parity = 0;
        for (size_t j = 0; j < m_checkNodes[i].bits.size(); j++) {
            size_t bitNum = m_checkNodes[i].bits[j]->bitNum;
            parity ^= LLRToBit(m_codeBits[bitNum].LLR.to_double());
        }
        if (parity) { //odd number of ones
//            std::cout << "equation #" << i << " does not meet parity!" << std::endl;
            ++result;
        }
    }
//    if (result > 0) {
//        std::cout << "Syndrome weight is " << result << std::endl;
//    }
    return result;
}

bool LDPCDecoder::LLRToBit(double llr) {
    return (llr < 0.0);
}

std::vector<bool> LDPCDecoder::getHardCodeWord() const
{
    std::vector<bool> message;
//    std::cout << "Assuming that the syndrome is 0, the message is: " << std::endl;

    for (size_t i = 0; i < m_codeBits.size(); i++) {
        message.push_back(LLRToBit(m_codeBits[i].softChannelEstimate.to_double()));
    }
    return  message;
}

void LDPCDecoder::printH() const
{
    std::cout << "H = " << std::endl;
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        for( unsigned j = 0; j < m_hcols; ++j)
        {
            std::cout << m_H[i][j] << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void LDPCDecoder::printSoftCodeWord() const
{
    std::cout << "CW LLR: ";
    for (size_t i = 0; i < m_codeBits.size(); i++) {
        std::cout << m_codeBits[i].LLR.to_double() << ", ";
    }
    std::cout << std::endl;
}

void LDPCDecoder::updateLLR()
{
    for (size_t i = 0; i < m_codeBits.size(); i++) {
        double llr = m_codeBits[i].softChannelEstimate.to_double();
        for (size_t j = 0; j < m_codeBits[i].checks.size(); ++j) {
            CheckNode *check = m_codeBits[i].checks[j];
            SLFixedPoint<LDPC_LLR_FORMAT> val = m_messages.at(GraphEdgeKey(check->checkNum, i));
            llr += (val).to_double();
        }
        m_codeBits[i].LLR = llr;
    }
}

//This operator is defined so that GraphEdgeKey can be used as a std::map key
bool operator<(const LDPCDecoder::GraphEdgeKey &lhs, const LDPCDecoder::GraphEdgeKey &rhs)
{
    if (lhs.checkNum == rhs.checkNum) {
        return (lhs.bitNum < rhs.bitNum);
    } else {
        return lhs.checkNum < rhs.checkNum;
    }
}

