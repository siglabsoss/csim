/*
 * fixedfir.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: ubuntu
 */

#include <fixedfir.h>

#define FILTER_LEN  63
#define MAX_INPUT_LEN   80// maximum length of filter than can be handled
#define MAX_FLT_LEN     63// buffer to hold all of the input samples
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)

// array to hold input samples

fixedfir::fixedfir(int N, FixedComplex<16>* tap, FilterChainElement *next) :
    FilterChainElement(next),
    m_n(N),
    m_taps(N),
    m_bench(N),
    m_output()
{
    for (int i = 0; i < N; i++) {
        m_taps[i] = tap[i];
    }
}

bool fixedfir::input(const block_io_t &data)
{

    //XXX convert data -> sample
    assert(data.type == IO_TYPE_FIXED_COMPLEX_16);
    FixedComplex<16> sample = data.fc;
    fir(sample);
    return true;
}
/**
 * output - provide an output sample to the caller.
 * @return false if no output sample is available.
 */
bool fixedfir::output(block_io_t &data)
{
    data.type = IO_TYPE_FIXED_COMPLEX_16;
    data.fc = m_output;
    return true;
}

void fixedfir::tick()
{

}

void fixedfir::fir(FixedComplex<16> &input)
{

    FixedComplex<32> sum;

    m_bench[0] = input; //New data on bench

    sum.real = 1 << 14;
    sum.imag = 1 << 14;
    for (int j = 0; (j < m_n); j++) {
        sum = sum + (m_bench[j].to_32() * m_taps[j].to_32());
    } //Accumulate

    for (int j = m_n - 1; j > 0; j--) {
        m_bench[j] = m_bench[j - 1];
    } //Moves all data down by 1 space

    sum = sum >> 15;
    m_output = sum.to_16();

}

fixedfir::~fixedfir()
{

}

