#include <complex>
#include <vector>
#include <mathlib/cordic.hpp>
#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <queue>
#include <core/logger.hpp>
#ifndef __FIXEDifft_H__
#define __FIXEDifft_H__

enum Fifft_STATE
{
    Fifft_STATE_INITIAL, Fifft_STATE_READ, Fifft_STATE_OUTPUT
};

class fixedifftbase : public FilterChainElement
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
    virtual void inputandtick(FixedComplex<32> x) = 0;
    virtual ~fixedifftbase();
    int ready;
    queue<FixedComplex<32> >   m_output;

};

class fixedifftstage: public fixedifftbase
{
public:
    int                  N;
    FixedComplex<32>    *memory;
    Fifft_STATE          state;
    int                 read_pointer;
    int                 write_pointer;
    int                 clock;
    fixedifftbase        *next;

    fixedifftstage(int Ninput);
    fixedifftstage();
    void init(int Ninput);
    void dump(void);
    void inputandtick(FixedComplex<32> x);
    void output(FixedComplex<32> x);
    void butterfly(FixedComplex<32> array[2], FixedComplex<32> x,
            FixedComplex<32> y);
    FixedComplex<32> twiddler(int k);

};
class fixedifftprint: public fixedifftbase
{
public:
    int N;
    int count;
    fixedifftprint(int Ninput);
    void inputandtick(FixedComplex<32> x);
};

// saves all output forever
class fixedifftbuffer: public fixedifftbase
{
public:
    int N;
    int count;
    std::vector<FixedComplex<32> > buf;
    fixedifftbuffer(int Ninput);
    void inputandtick(FixedComplex<32> x);
};

class fixedifft: public fixedifftbase
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
    fixedifftstage *stages;
    fixedifftprint printer;
    fixedifft(int Ninput);
    void inputandtick(FixedComplex<32> x);
};

#endif
