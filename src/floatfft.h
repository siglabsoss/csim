#include <complex>
#include <vector>

#ifndef __FLOATFFT_H__
#define __FLOATFFT_H__

enum FFT_STATE
{
    FFT_STATE_INITIAL, FFT_STATE_READ, FFT_STATE_OUTPUT
};

class floatfftbase
{
public:
    virtual void inputandtick(complex<float> x) = 0;
    int ready;
};

class floatfftstage: public floatfftbase
{
public:
    int N;
    complex<float> *memory;
    FFT_STATE state;

    int read_pointer;
    int write_pointer;
    int clock;

    floatfftbase *next;

    floatfftstage(int Ninput);
    floatfftstage();
    void init(int Ninput);
    void dump(void);
    void inputandtick(complex<float> x);
    void output(complex<float> x);
    void butterfly(complex<float> array[2], complex<float> x, complex<float> y);
    complex<float> twiddler(int k);
};

class floatfftprint: public floatfftbase
{
public:
    int N;
    int count;
    floatfftprint(int Ninput);
    void inputandtick(complex<float> x);
};

// saves all output forever
class floatfftbuffer: public floatfftbase
{
public:
    int N;
    int count;
    std::vector<complex<float> > buf;
    floatfftbuffer(int Ninput);
    void inputandtick(complex<float> x);
};

class floatfft: public floatfftbase
{
public:
    int N;
    int stagecount;
    floatfftstage *stages;
    floatfftprint printer;
    floatfft(int Ninput);
    void inputandtick(complex<float> x);
};

#endif
