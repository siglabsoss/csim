#include <complex>
#include <vector>
#include <mathlib/cordic.hpp>
#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <queue>
#include <cmath>
#include <iomanip>
#include <core/logger.hpp>
#ifndef __FIXEDFFT_H__
#define __FIXEDFFT_H__

typedef FixedComplex32 fft_complex_t;

enum FFFT_STATE
{
    FFFT_STATE_INITIAL, FFFT_STATE_READ, FFFT_STATE_OUTPUT
};

class fixedfftbase : public FilterChainElement
{
public:
    int m_count;
    bool input(const filter_io_t &data) override;

        /**
         * output - provide an output sample to the caller.
         * @return false if no output sample is available.
         */
    bool output(filter_io_t &data) override;

    void tick() override;
    virtual void inputandtick(fft_complex_t x) = 0;
    virtual ~fixedfftbase();
    int ready;
    queue<fft_complex_t >   m_output;

};

class fixedfftstage: public fixedfftbase
{
public:
    int                  N;
    fft_complex_t    *memory;
    FFFT_STATE          state;
    int                 read_pointer;
    int                 write_pointer;
    int                 clock;
    fixedfftbase        *next;
    int theta;
    int tableSize;
    int* mainTablePointer;
    fixedfftstage(int Ninput);
    fixedfftstage();
    void init(int Ninput);
    void dump(void);
    void inputandtick(fft_complex_t x);
    void output(fft_complex_t x);
    void butterfly(fft_complex_t array[2], fft_complex_t x,
            fft_complex_t y);
    fft_complex_t twiddler(int k);


};
class fixedfftprint: public fixedfftbase
{
public:
    int N;
    int count;
    fixedfftprint(int Ninput);
    void inputandtick(fft_complex_t x);
};

// saves all output forever
class fixedfftbuffer: public fixedfftbase
{
public:
    int N;
    int count;
    std::vector<fft_complex_t > buf;
    fixedfftbuffer(int Ninput);
    void inputandtick(fft_complex_t x);
};

class fixedfft: public fixedfftbase
{
public:
    bool input(const filter_io_t &data) override;

        /**
         * output - provide an output sample to the caller.
         * @return false if no output sample is available.
         */
    bool output(filter_io_t &data) override;

    void tick() override;
    int N;
    int stagecount;
    //int* mainTable;
    int* mainTable;
    fixedfftstage *stages;
    fixedfftprint printer;
    fixedfft(int Ninput, int tableSize = 0);
    void inputandtick(fft_complex_t x);
};

#endif
