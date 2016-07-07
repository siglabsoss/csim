#include <complex>

#ifndef __FLOATFFT_H__
#define __FLOATFFT_H__


enum FFT_STATE {
	FFT_STATE_INITIAL,
	FFT_STATE_READ,
	FFT_STATE_OUTPUT,
	FFT_STATE_IDLE
};

class floatfftbase {
public:
	virtual void inputandtick(complex<float> x) = 0;
};

class floatfftstage : public floatfftbase {
public:
	int N;
	complex<float> *memory;
	FFT_STATE state;

	int read_pointer;
	int write_pointer;
	int clock;

	floatfftbase *next;

	floatfftstage(int Ninput);
	void dump(void);
	void inputandtick(complex<float> x);
	void output(complex<float> x);
	void butterfly(complex<float> array[2], complex<float> x, complex<float> y);
	complex<float> twiddler(int k);
};

class floatfftprint : public floatfftbase {
public:
	int N;
	int count;
	floatfftprint(int Ninput);
	void inputandtick(complex<float> x);
};


#endif
