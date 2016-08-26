#include <complex>
#include <vector>
#include <queue>

#ifndef __FLOATFFT_H__
#define __FLOATFFT_H__
enum FFT_STATE
{
    FFT_STATE_INITIAL, FFT_STATE_READ, FFT_STATE_OUTPUT
};

class floatfftbase
{
public:
    virtual void inputandtick(std::complex<float> x) {};
    int ready;
};

class floatfftstage: public floatfftbase
{
public:
    int N;
    std::complex<float> *memory;
    FFT_STATE state;

    int read_pointer;
    int write_pointer;
    int clock;

    floatfftbase *next;

    floatfftstage(int Ninput);
    floatfftstage();
    void init(int Ninput);
    void dump(void);
    void inputandtick(std::complex<float> x);
    void output(std::complex<float> x);
    void butterfly(std::complex<float> array[2], std::complex<float> x, std::complex<float> y);
    std::complex<float> twiddler(int k);
};

class floatfftprint: public floatfftbase
{
public:
    int N;
    int count;
    floatfftprint(int Ninput);
    void inputandtick(std::complex<float> x);
    std::queue<std::complex<float> > answers;
};

// saves all output forever
class floatfftbuffer: public floatfftbase
{
public:
    int N;
    int count;
    std::vector<std::complex<float> > buf;
    floatfftbuffer(int Ninput);
    void inputandtick(std::complex<float> x);
};

class floatfft: public floatfftbase
{
public:
    int N;
    int stagecount;
    floatfftstage *stages;
    floatfftprint printer;
    floatfft(int Ninput);
    void inputandtick(std::complex<float> x);
};

#endif
