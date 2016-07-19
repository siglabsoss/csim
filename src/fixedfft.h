#include <complex>
#include <vector>
#include "fixedcomplex.h"

#ifndef __FIXEDFFT_H__
#define __FIXEDFFT_H__

enum FFFT_STATE
{
    FFFT_STATE_INITIAL, FFFT_STATE_READ, FFFT_STATE_OUTPUT
};

class fixedfftbase
{
public:
    virtual void inputandtick(FixedComplex<32> x) = 0;
    int ready;
};

class fixedfftstage: public fixedfftbase
{
public:
    int N;
    FixedComplex<32> *memory;
    FFFT_STATE state;

    int read_pointer;
    int write_pointer;
    int clock;

    fixedfftbase *next;

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
    int N;
    int stagecount;
    fixedfftstage *stages;
    fixedfftprint printer;
    fixedfft(int Ninput);
    void inputandtick(FixedComplex<32> x);
};

#endif
