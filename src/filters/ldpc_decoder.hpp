#pragma once

#include <core/filter_chain_element.hpp>
#include <types/fixedpoint.hpp>

#include <vector>
#include <map>

#define LDPC_LLR_WL 16
#define LDPC_LLR_IWL 8
#define LDPC_LLR_FORMAT LDPC_LLR_WL, LDPC_LLR_IWL, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

class LDPCDecoder : public FilterChainElement
{
public:
    virtual ~LDPCDecoder();
    LDPCDecoder(const std::vector<std::vector<bool> > &H);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;

    void printH()           const;
    void printSoftCodeWord()             const;
    std::vector<bool> getHardCodeWord() const;
    void decode(const std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > &cw, size_t iterations, bool& solved, size_t& solved_iterations);
public:
    struct GraphEdgeKey
    {
        size_t checkNum;
        size_t bitNum;
    };
private:
    struct CheckNode;
    struct BitNode
    {
        SLFixedPoint<LDPC_LLR_FORMAT> softChannelEstimate;
        SLFixedPoint<LDPC_LLR_FORMAT> LLR;
        size_t                        bitNum;
        std::vector<CheckNode *>      checks;
    };

    struct CheckNode
    {
        size_t                        checkNum;
        std::vector<BitNode *>        bits;

    };

    void parseH();
    size_t parityCheck() const;
    void updateLLR();
    void iteration();
    static bool LLRToBit(double llr);

    uint32_t m_hrows;
    uint32_t m_hcols;
    std::vector<std::vector<bool> > m_H;
    std::vector<BitNode>   m_codeBits;
    std::vector<CheckNode> m_checkNodes;
    std::map<GraphEdgeKey, SLFixedPoint<LDPC_LLR_FORMAT> > m_messages; //store LLR messages per edge
    std::map<GraphEdgeKey, SLFixedPoint<LDPC_LLR_FORMAT> > m_tmpMsgs;   //store LLR messages per edge
};

bool operator<(const LDPCDecoder::GraphEdgeKey &lhs, const LDPCDecoder::GraphEdgeKey &rhs);
