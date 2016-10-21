
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
    // calculate degree for each check node, fill in node_index
    for( unsigned i = 0; i < m_hrows; ++i )
    {
        for( unsigned j = 0; j < m_hcols; ++j)
        {
            if( m_H[i][j] )
            {
                //For each parity check node, build an adjacency list of code bits involved in the
                //parity check equation
                BitNode *bit = &m_codeBits[j];
                bit->bitNum = j;
                m_checkNodes[i].bits.push_back(bit);
            }
        }

        std::cout << "Check node " << i << " has degree " << m_checkNodes[i].bits.size() << std::endl;
    }
}

void LDPCDecoder::iteration()
{
    //For every check node, scan the connected code bits and store the one with the minimum LLR magnitude.
    //Also track odd or even parity by, in this case, multiplying sign bits.
    for (size_t i = 0; i < m_checkNodes.size(); i++) {
        assert(m_checkNodes[i].bits.size() >= 2);
//        double minMag = std::min(std::abs(m_checkNodes[i].bits[0]->llr.to_double()), std::abs(m_checkNodes[i].bits[1]->llr.to_double()));
//        double sign = (m_checkNodes[i].bits[0]->llr.to_double() < 0.0) ? -1 : 1;
//        sign *= (m_checkNodes[i].bits[1]->llr.to_double() < 0.0) ? -1 : 1;

//        for (size_t j = 2; j < m_checkNodes[i].bits.size(); j++) {
//            minMag = std::min(minMag, std::abs(m_checkNodes[i].bits[j]->llr.to_double()));
//            sign *= (m_checkNodes[i].bits[j]->llr.to_double() < 0.0) ? -1 : 1;
//        }
        CheckNode &currentCheckNode = m_checkNodes[i];
        std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > interimLLR(m_codeBits.size());
        for (size_t j = 0; j < currentCheckNode.bits.size(); j++) {
            double minMag = static_cast<double>(1ull << LDPC_LLR_IWL); //starting with larger magnitude than possible
            double sign = 1.0;
//            std::cout << "finding min of ";
            for (size_t k = 0; k < currentCheckNode.bits.size(); k++) {
                if (j == k) {
                    continue;
                }
//                std::cout << m_checkNodes[i].bits[k]->llr.to_double() << " ";
                minMag = std::min(minMag, std::abs(currentCheckNode.bits[k]->llr.to_double()));
                sign *= (currentCheckNode.bits[k]->llr.to_double() < 0.0) ? -1.0 : 1.0;
            }
//            std::cout << std::endl;
//            std::cout << "minMag for check node " << i << " being sent back to bit " << currentCheckNode.bits[j]->bitNum << " is " << minMag << std::endl;
            SLFixedPoint<LDPC_LLR_FORMAT> val(sign * minMag);
            interimLLR[currentCheckNode.bits[j]->bitNum] += val;
            //currentCheckNode.bits[j]->llr += (val);
        }

        for (size_t i = 0; i < m_codeBits.size(); i++) {
            m_codeBits[i].llr += interimLLR[i];
        }
    }

    //Update each code bit's LLR with the sum of all of the LLRs from connected check nodes
//    for (size_t i = 0; i < m_checkNodes.size(); i++) {
//        for (size_t j = 0; j < m_checkNodes[i].bits.size(); j++) {
//            size_t bitNum = m_checkNodes[i].bits[j]->bitNum;
//            m_codeBits[bitNum].llr += m_checkNodes[i].llr;
//        }
//    }
}

size_t LDPCDecoder::parityCheck() const
{
    size_t result = 0;
    for (size_t i = 0; i < m_checkNodes.size(); i++) {
        size_t numOnes = 0;
        for (size_t j = 0; j < m_checkNodes[i].bits.size(); j++) {
            size_t bitNum = m_checkNodes[i].bits[j]->bitNum;
            bool val = (LLRToBit(m_codeBits[bitNum].llr.to_double()));
//            if (i == 0) {
//                std::cout << "bit #" << bitNum << " = " << (int)val << std::endl;
//            }
            if (val) {
                numOnes++;
            }
        }
        if (numOnes & 1) { //odd number of ones
            std::cout << "equation #" << i << " has an odd number of ones (" << numOnes << ")" << std::endl;
            ++result;
        }
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
        print_cw_llr();
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
    for (size_t i = 0; i < m_codeBits.size(); i++) {
        std::cout << m_codeBits[i].llr << ", ";
    }
    std::cout << std::endl;
}

