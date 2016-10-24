
#include <filters/ldpc_decoder.hpp>

#include <core/parameters.hpp>

#include <cassert>


bool LDPCDecoder::input(const filter_io_t &data)
{

    return true;
}

bool LDPCDecoder::output(filter_io_t &data)
{

    return true;
}

void LDPCDecoder::tick(void)
{

}

LDPCDecoder::LDPCDecoder(const std::vector<std::vector<bool> > &H):
FilterChainElement(std::string("LDPCDecoder")),
m_hrows(H.size()),
m_hcols(H[0].size()), //will crash if hrows == 0
m_H(H),
m_codeBits(m_hcols),
m_checkNodes(m_hrows)
{
    parse_H_matrix();
}

LDPCDecoder::~LDPCDecoder()
{

}


void LDPCDecoder::parse_H_matrix()
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
                GraphEdgeKey key = {
                        .checkNum = i,
                        .bitNum = j
                };
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
        for (size_t i = 0; i < m_codeBits[bit].checks.size(); ++i) {
            CheckNode *targetCheck = m_codeBits[bit].checks[i];
            SLFixedPoint<LDPC_LLR_FORMAT> llr = m_codeBits[bit].llr;
            for (size_t j = 0; j < m_codeBits[bit].checks.size(); ++j) {
                if (i == j) { //skip the check node we're calculating an updated value for
                    continue;
                }
                CheckNode *otherCheck = m_codeBits[bit].checks[j];
                GraphEdgeKey key = {
                    .checkNum = otherCheck->checkNum,
                    .bitNum = bit
                };
                //std::cout << m_messages[key] << std::endl;
//                std::cout << llr.to_double() << " + " << m_messages[key].to_double();
                llr = llr.to_double() + m_messages[key].to_double();
//                std::cout << " = " << llr.to_double() << std::endl;

            }
            GraphEdgeKey key = {
                    .checkNum = targetCheck->checkNum,
                    .bitNum = bit
            };
//            std::cout << "Bits to Checks (" << key.bitNum << "," << key.checkNum << ") " << m_messages[key].to_double() << " -> " << llr.to_double() << std::endl;
            m_tmpMsgs[key] = llr;
        }
        m_messages = m_tmpMsgs;
    }
    //Checks-to-bits pass: Create messages for each edge by taking the minimum...
    for (size_t check = 0; check < m_checkNodes.size(); ++check) {
        for (size_t i = 0; i < m_checkNodes[check].bits.size(); ++i) {
            BitNode *targetBit = m_checkNodes[check].bits[i];
            double minMag = static_cast<double>(1ull << LDPC_LLR_IWL); //starting with larger magnitude than possible
            double sign = 1.0;
            for (size_t j = 0; j < m_checkNodes[check].bits.size(); ++j) {
                if (i == j) {
                    continue;
                }
                BitNode *otherBit = m_checkNodes[check].bits[j];
                GraphEdgeKey key = {
                        .checkNum = check,
                        .bitNum = otherBit->bitNum
                };
                minMag = std::min(minMag, std::abs(m_messages[key].to_double()));
                sign *= (m_messages[key].to_double() < 0.0) ? -1.0 : 1.0;
            }
            GraphEdgeKey key = {
                    .checkNum = check,
                    .bitNum = targetBit->bitNum
            };
            SLFixedPoint<LDPC_LLR_FORMAT> val(sign * minMag);
//            std::cout << "Checks to Bits (" << key.bitNum << "," << key.checkNum << ") " << m_messages[key].to_double() << " -> " << val.to_double() << std::endl;
            m_tmpMsgs[key] = val;
        }
        m_messages = m_tmpMsgs;
    }

}

size_t LDPCDecoder::parityCheck() const
{
    std::vector<double> cwLLR;
    calculate_llr(cwLLR);
    size_t result = 0;
    for (size_t i = 0; i < m_checkNodes.size(); i++) {
        size_t parity = 0;
        for (size_t j = 0; j < m_checkNodes[i].bits.size(); j++) {
            size_t bitNum = m_checkNodes[i].bits[j]->bitNum;
            parity ^= LLRToBit(cwLLR[bitNum]);
//            if (i == 0) {
//                std::cout << "bit #" << bitNum << " = " << (int)val << std::endl;
//            }
        }
        if (parity) { //odd number of ones
            std::cout << "equation #" << i << " does not meet parity!" << std::endl;
            ++result;
        }
    }
    if (result > 0) {
        std::cout << "Syndrome weight is " << result << std::endl;
    }
    return result;
}

void LDPCDecoder::decode(const std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > &cw, size_t iterations, bool& solved, size_t& solved_iterations)
{
    assert(cw.size() == m_codeBits.size());
    assert(m_codeBits.size() == m_hcols);

    for (size_t i = 0; i < m_codeBits.size(); i++) {
        m_codeBits[i].llr = cw[i];
    }

    // These stay here if code is unsolved
    solved = false;
    solved_iterations = 0;

    for(size_t i = 0; i < iterations; ++i)
    {
        bool justSolved = (parityCheck()== 0);
        if (justSolved && !solved) {
            solved = true;
            solved_iterations = i;
        }

        if (!justSolved && solved) {
            assert(true);
        }
        iteration();

    }
}

bool LDPCDecoder::LLRToBit(double llr) {
    return (llr < 0.0);
}

std::vector<bool> LDPCDecoder::get_message() const
{
    std::vector<bool> message;
//    std::cout << "Assuming that the syndrome is 0, the message is: " << std::endl;

    for (size_t i = 0; i < m_codeBits.size(); i++) {
        message.push_back(LLRToBit(m_codeBits[i].llr.to_double()));
    }
    return  message;
}

void LDPCDecoder::print_H_matrix() const
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

void LDPCDecoder::print_cw_llr() const
{
    std::vector<double> cwLLR;
    calculate_llr(cwLLR);
    std::cout << "CW LLR: ";
    for (size_t i = 0; i < cwLLR.size(); i++) {
        std::cout << cwLLR[i] << ", ";
    }
    std::cout << std::endl;
}

void LDPCDecoder::calculate_llr(std::vector<double> &cwLLR) const
{
    cwLLR.resize(m_codeBits.size());
    for (size_t i = 0; i < m_codeBits.size(); i++) {
        double llr = 0.0;// = m_codeBits[i].llr.to_double();
        for (size_t j = 0; j < m_codeBits[i].checks.size(); ++j) {
            CheckNode *check = m_codeBits[i].checks[j];
            GraphEdgeKey key;
            key.bitNum = i;
            key.checkNum = check->checkNum;
            SLFixedPoint<LDPC_LLR_FORMAT> val = m_messages.at(key);
            llr += (val).to_double();
        }
        cwLLR[i] = llr + m_codeBits[i].llr.to_double();
    }
}

bool operator<(const LDPCDecoder::GraphEdgeKey &lhs, const LDPCDecoder::GraphEdgeKey &rhs)
{
    if (lhs.checkNum == rhs.checkNum) {
        return (lhs.bitNum < rhs.bitNum);
    } else {
        return lhs.checkNum < rhs.checkNum;
    }
}

