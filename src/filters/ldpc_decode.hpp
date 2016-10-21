#pragma once


#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>

#define LDPC_LLR_FORMAT 16, 4, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE

typedef struct
{
    SLFixedPoint<LDPC_LLR_FORMAT> llr;
} LDPC_N;

typedef struct
{
    SLFixedPoint<LDPC_LLR_FORMAT> llr;
    unsigned degree;

    // index 0 is the smallest, 1 is the 2nd smallest
    SLFixedPoint<LDPC_LLR_FORMAT> min[2];
    unsigned min_index[2];

    // static defined for now
    std::vector<unsigned> node_index;

    // 0 means that this check node is satisfied
    short parity;

} LDPC_M;

class LDPCDecode : public FilterChainElement
{
public:
    virtual ~LDPCDecode();
    LDPCDecode(const std::vector<std::vector<bool> > &H);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;


    void print_h();
    unsigned get_syndrome(void);
    void calc_syndrome(unsigned print);
    void run();
    std::vector<bool> get_message();
    void print_cw();
    void decode(const std::vector<SLFixedPoint<LDPC_LLR_FORMAT> > &cw, size_t iterations, bool& solved, size_t& solved_iterations);
private:

    void prep_once();
    unsigned check_equations(void);
    void iteration();

    uint32_t m_hrows;
    uint32_t m_hcols;
    std::vector<std::vector<bool> > m_H;
    std::vector<LDPC_N> m_n;
    std::vector<LDPC_M> m_m;
};


