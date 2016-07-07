#include <iostream>

using namespace std;


#include "floatfft.h"
const float pi = 3.14159265359;

floatfftstage::floatfftstage(int Ninput)
{
	N = Ninput;

	memory = new complex<float>[N/2];

	state = FFT_STATE_INITIAL;

	read_pointer = 0;
	write_pointer = 0;

	ready = 1;

	clock = 0;

	cout << "FFT bufferfly " << N << " was created" << endl;
}

void floatfftstage::dump(void)
{
	int i;
	for(i=0;i<N/2;i++)
	{
		cout << "Memory " << i << ": " << memory[i] << endl;
	}
}

void floatfftstage::butterfly(complex<float> array[2], complex<float> x, complex<float> y)
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
	W_sin = -sin(theta);

	complex<float> W(W_cos, W_sin);
	return W;

}

void floatfftstage::output(complex<float> x)
{
//	cout << "FFT(" << N << ") passing along " << x << endl;
	next->inputandtick(x);
	while(!next->ready)
	{
		next->inputandtick(0);
	}
}

void floatfftstage::inputandtick(complex<float> x){

//	cout << "FFT(" << N << ") starting in state " << state << " with x " << x << endl;

	if( state != FFT_STATE_OUTPUT && x == complex<float>(0,0))
	{
		cout << "PROBLEMS " << endl;
	}

	complex<float> butterflyresult[2];
	complex<float> outputtemp;

	int i;

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


		butterfly(butterflyresult, memory[read_pointer], x);
		memory[write_pointer] = butterflyresult[1];

		//		cout << "butterfly output " << butterflyresult[0] << endl;
		output(butterflyresult[0]);



		if (read_pointer == ((N/2)-1))
		{
			state = FFT_STATE_OUTPUT;
			write_pointer = 0;
			read_pointer = 0;
		}
		else
		{
			read_pointer++;
			write_pointer++;
		}

		break;
	case FFT_STATE_OUTPUT:

		outputtemp = memory[read_pointer] * twiddler(read_pointer);
		output(outputtemp);

		if (read_pointer == ((N/2)-1))
		{

			state = FFT_STATE_INITIAL;
			write_pointer = 0;
			read_pointer = 0;
		}
		else
		{
			read_pointer++;
		}

		break;

	case FFT_STATE_IDLE:
		break;
	}

	clock++;
	ready = (state != FFT_STATE_OUTPUT);

}




floatfftprint::floatfftprint(int Ninput)
{
	N = Ninput;
	count = 0;
}

void floatfftprint::inputandtick(complex<float> x)
{
	cout << "output[" << count << "]: " << x << endl;
	count++;
}

