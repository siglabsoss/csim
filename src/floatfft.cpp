#include <iostream>

using namespace std;


#include "floatfft.h"


floatfft::floatfft(int Ninput)
{
	this->N = Ninput;

	this->memory = new float[this->N];

	this->state = FFT_STATE_INITIAL;

	this->read_pointer = 0;
	this->write_pointer = 0;

	cout << "FFT bufferfly " << this->N << " was created" << endl;
}

void floatfft::dump(void)
{
	int i;
	for(i=0;i<N;i++)
	{
		cout << "Memory " << i << ": " << memory[i] << endl;
	}
}


void floatfft::inputandtick(float x){

	cout << "starting in state" << state << endl;

	switch(state)
	{
	default:
	case FFT_STATE_INITIAL:

		write_pointer++;

		if (write_pointer == ((N/2)-1))
		{
			state = FFT_STATE_READ;
		}
		break;
	case FFT_STATE_READ:
		break;
	case FFT_STATE_OUTPUT:
		break;
	}







}
void floatfft::compute(void){

}
