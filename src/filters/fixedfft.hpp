#include <complex>
#include <vector>
#include <mathlib/cordic.hpp>
#include <core/filter_chain_element.hpp>
#include <types/fixedcomplex.hpp>
#include <queue>
#include <core/logger.hpp>
#ifndef __FIXEDFFT_H__
#define __FIXEDFFT_H__

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
    virtual void inputandtick(FixedComplex<32> x) = 0;
    virtual ~fixedfftbase();
    int ready;
    queue<FixedComplex<32> >   m_output;

};

class fixedfftstage: public fixedfftbase
{
public:
    int                  N;
    FixedComplex<32>    *memory;
    FFFT_STATE          state;
    int                 read_pointer;
    int                 write_pointer;
    int                 clock;
    fixedfftbase        *next;

    fixedfftstage(int Ninput);
    fixedfftstage();
    void init(int Ninput);
    void dump(void);
    void inputandtick(FixedComplex<32> x);
    void output(FixedComplex<32> x);
    void butterfly(FixedComplex<32> array[2], FixedComplex<32> x,
            FixedComplex<32> y);
    FixedComplex<32> twiddler(int k);

};
class fixedfftprint: public fixedfftbase
{
public:
    int N;
    int count;
    fixedfftprint(int Ninput);
    void inputandtick(FixedComplex<32> x);
};

// saves all output forever
class fixedfftbuffer: public fixedfftbase
{
public:
    int N;
    int count;
    std::vector<FixedComplex<32> > buf;
    fixedfftbuffer(int Ninput);
    void inputandtick(FixedComplex<32> x);
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
    fixedfftstage *stages;
    fixedfftprint printer;
    fixedfft(int Ninput);
    void inputandtick(FixedComplex<32> x);
};

#endif
