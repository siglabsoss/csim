#ifndef __FLOATFFT_H__
#define __FLOATFFT_H__


enum FFT_STATE {
	FFT_STATE_INITIAL,
	FFT_STATE_READ,
	FFT_STATE_OUTPUT
};


class floatfftstage {
public:
	int N;
	float *memory;
	FFT_STATE state;

	int read_pointer;
	int write_pointer;

	floatfftstage(int Ninput);
	void dump(void);
	void inputandtick(float x);
	void compute(void);

};


#endif
