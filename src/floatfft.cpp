#include <iostream>

using namespace std;


#include "floatfft.h"


floatfft::floatfft(int Ninput)
{
	this->N = Ninput;

	this->memory = new float[this->N];

	cout << "FFT bufferfly " << this->N << " was created" << endl;
}

void floatfft::dump(void)
{
	int i;
	for(i=0;i<this->N;i++)
	{
		cout << "Memory " << i << ": " << this->memory[i] << endl;
	}
}
