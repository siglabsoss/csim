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

enum FFFT_STATE
{
    FFFT_STATE_INITIAL, FFFT_STATE_READ, FFFT_STATE_OUTPUT
};

class fixedfftbase
{
public:
    virtual void inputandtick(FixedComplex32 x) = 0;
    virtual ~fixedfftbase() {}
    int ready;
    queue<FixedComplex32 >   m_output;

};

class fixedfftstage: public fixedfftbase
{
public:
    int                  N;
    std::vector<FixedComplex32>    memory;
    FFFT_STATE          state;
    int                 read_pointer;
    int                 write_pointer;
    int                 clock;
    fixedfftbase        *next;
    int theta;
    int tableSize;
    std::vector<int> *mainTablePointer;

    fixedfftstage();
    void init(int Ninput);
    void dump(void);
    void inputandtick(FixedComplex32 x);
    void output(FixedComplex32 x);
    void butterfly(FixedComplex32 array[2], FixedComplex32 x,
            FixedComplex32 y);
    FixedComplex32 twiddler(int k);


};
class fixedfftprint: public fixedfftbase
{
public:
    int N;
    int count;
    fixedfftprint(int Ninput);
    void inputandtick(FixedComplex32 x);
};

class fixedfft : public FilterChainElement
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
    bool newInput;
    int m_count;
    std::vector<int> mainTable;

    std::vector<fixedfftstage> stages;
    fixedfftprint printer;
    fixedfft(int Ninput, int tableSize = 0);
};

#endif
