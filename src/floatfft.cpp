#include <iostream>

using namespace std;


#include "floatfft.h"
const float pi = 3.14159265359;

floatfftstage::floatfftstage(int Ninput)
{
	this->N = Ninput;

	this->memory = new float[this->N/2];

	this->state = FFT_STATE_INITIAL;

	this->read_pointer = 0;
	this->write_pointer = 0;

	cout << "FFT bufferfly " << this->N << " was created" << endl;
}

void floatfftstage::dump(void)
{
	int i;
	for(i=0;i<N/2;i++)
	{
		cout << "Memory " << i << ": " << memory[i] << endl;
	}
}

void floatfftstage::butterfly(float array[2], float x, float y)
{
	array[0] = x + y;
	array[1] = x - y;
}

complex<float> floatfftstage::twiddler(int k)
{

	float W_cos;
	float W_sin;
	float theta;
	theta = (2 * pi/N) * k;
	W_cos = cos(theta);
	W_sin = sin(theta);

	complex<float> W(W_cos, W_sin);
	return W;

}


void floatfftstage::inputandtick(float x){

	cout << "starting in state" << state << endl;

	switch(state)
	{
	default:
	case FFT_STATE_INITIAL:

		memory[write_pointer] = x;


		if (write_pointer == ((N/2)-1))
		{
			state = FFT_STATE_READ;
			write_pointer = 0;
		}
		else
		{
			write_pointer++;
		}


		break;
	case FFT_STATE_READ:

		float butterflyresult[2];
		butterfly(butterflyresult, memory[write_pointer], x);
		memory[write_pointer] = butterflyresult[1];
		cout << "butterfly output " << butterflyresult[0] << endl;
		write_pointer++;

		break;
	case FFT_STATE_OUTPUT:
		break;
	}







}
void floatfftstage::compute(void){

}
