#pragma once


#include <iostream>
#include <cassert>

#include <core/filter_chain_element.hpp>
#include <utils/plotter.hpp>


//
//
//typedef struct
//{
//    float llr;
//} LDPC_N;
//
//typedef struct
//{
//    float llr;
//    unsigned degree;
//
//    // index 0 is the smallest, 1 is the 2nd smallest
//    float min[2];
//    float min_index[2];
//
//    // static defined for now
//    vector<unsigned> node_index;
//
//    // 0 means that this check node is satisfied
//    short parity;
//
//} LDPC_M;

class LDPCEncode : public FilterChainElement
{
public:
    virtual ~LDPCEncode();
    LDPCEncode(vector<vector<uint8_t> > G, uint32_t rows, uint32_t cols);
    bool input(const filter_io_t &data) override;
    bool output(filter_io_t &data) override;
    void tick(void) override;


//    void print_h();
//    unsigned get_syndrome(void);
//    void calc_syndrome(unsigned print);
//    void run();
//    vector<uint8_t> get_message();
//    void print_cw();
//    void decode(vector<int> cw, size_t iterations, bool& solved, size_t& solved_iterations);
    vector<uint8_t> encode(vector<uint8_t> u);
private:

    void prep_once();
    unsigned check_equations(void);
    void iteration();


    uint32_t m_hrows;
    uint32_t m_hcols;
    vector<vector<uint8_t> > m_G;
//    vector<LDPC_N> m_n;
//    vector<LDPC_M> m_m;


private:
//    ComplexDouble m_carrier;
//    ComplexDouble m_input;
//
//    size_t m_count;
//    uint32_t m_ticksPerPeriod;
//    bool    m_inputValid;
//    bool    m_upMix;
};


