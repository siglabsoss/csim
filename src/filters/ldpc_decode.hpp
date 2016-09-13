#pragma once


#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>



#define LARGER_THAN_COLS (25)
#define h_rows (16)
#define h_cols (24)



typedef struct
{
    float llr;
} LDPC_N;

typedef struct
{
    float llr;
    unsigned degree;

    // index 0 is the smallest, 1 is the 2nd smallest
    float min[2];
    float min_index[2];

    // static defined for now
    unsigned node_index[LARGER_THAN_COLS];

    // 0 means that this check node is satisfied
    short parity;

} LDPC_M;

class LDPCDecode : public FilterChainElement
{
public:
    virtual ~LDPCDecode();
    LDPCDecode(vector<vector<uint8_t> > H, uint32_t rows, uint32_t cols);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;


    void print_h();
    unsigned get_syndrome(void);
    void calc_syndrome(void);
    void run();
    void get_message();
    void print_cw();
    void decode(vector<int> cw, size_t iterations);
private:

    void prep_once();
    unsigned check_equations(void);
    void iteration();


    uint32_t m_hrows;
    uint32_t m_hcols;
    vector<vector<uint8_t> > m_H;
    vector<LDPC_N> m_n;
    vector<LDPC_M> m_m;


private:
//    ComplexDouble m_carrier;
//    ComplexDouble m_input;
//
//    size_t m_count;
//    uint32_t m_ticksPerPeriod;
//    bool    m_inputValid;
//    bool    m_upMix;
};


