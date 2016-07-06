#ifndef __FLOATFFT_H__
#define __FLOATFFT_H__



class floatfft {
public:
	int N;
	float *memory;

	floatfft(int Ninput);
	void dump(void);

};


#endif
