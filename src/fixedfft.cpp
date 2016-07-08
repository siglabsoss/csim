#include <iostream>
#include <cmath>
#include "fixedcomplex.h"
using namespace std;


#include "fixedfft.h"
const float pi = 3.14159265359;

void fixedfftstage::init(int Ninput)
{
	N = Ninput;

	memory = new FixedComplex<32> [N/2];

	state = FFFT_STATE_INITIAL;

	read_pointer = 0;
	write_pointer = 0;

	ready = 1;

	clock = 0;

//	cout << "FFT bufferfly " << N << " was created" << endl;
}

fixedfftstage::fixedfftstage(int Ninput)
{
	init(Ninput);
}

fixedfftstage::fixedfftstage()
{

}

void fixedfftstage::dump(void)
{
	int i;
	for(i=0;i<N/2;i++)
	{
		cout << "Memory " << i << ": " << memory[i] << endl;
	}
}

void fixedfftstage::butterfly(FixedComplex<32>  array[2], FixedComplex<32>  x, FixedComplex<32>  y)
{
	array[0] = x + y;
	array[1] = x - y;
}

FixedComplex<32>  fixedfftstage::twiddler(int k)
{
	int a = pow(2,15);
	int scale = a/1000.0;

	float W_cos;
	float W_sin;
	float theta;
	theta = (2 * pi/N) * k;
	sc_int<32> as_cos, as_sin;
	W_cos = cos(theta);
	W_cos = W_cos * scale;
	W_sin = -sin(theta);
	W_sin = W_sin * scale;

	FixedComplex<32> W(W_cos, W_sin);
	return W;

}

void fixedfftstage::output(FixedComplex<32> x)
{
	while(!next->ready)
	{
		cout << N << "X" << endl;
		next->inputandtick(FixedComplex<32>(0,0));
	}
	next->inputandtick(x);
}

void fixedfftstage::inputandtick(FixedComplex<32> x){

	//	cout << "FFT(" << N << ") starting in state " << state << " with x " << x << endl;

	if( state != FFFT_STATE_OUTPUT && x == FixedComplex<32>(0,0))
	{
		cout << "PROBLEMS " << endl;
	}

	FixedComplex<32> butterflyresult[2];
	FixedComplex<32> outputtemp;

	int i;
	int as;
	int a = pow(2,15);
	int scale = a/1000.0;

	switch(state)
	{
	default:
	case FFFT_STATE_INITIAL:
		cout << N << "a" << endl;

		memory[write_pointer] = x;


		if (write_pointer == ((N/2)-1))
		{
			state = FFFT_STATE_READ;
			write_pointer = 0;
		}
		else
		{
			write_pointer++;
		}


		break;
	case FFFT_STATE_READ:
		cout << N << "b" << endl;


		butterfly(butterflyresult, memory[read_pointer], x);
		memory[write_pointer] = butterflyresult[1];

		//		cout << "butterfly output " << butterflyresult[0] << endl;
		output(butterflyresult[0]);



		if (read_pointer == ((N/2)-1))
		{
			state = FFFT_STATE_OUTPUT;
			write_pointer = 0;
			read_pointer = 0;
		}
		else
		{
			read_pointer++;
			write_pointer++;
		}

		break;
	case FFFT_STATE_OUTPUT:
		cout << N << "c" << endl;
		if(N == 8)
		{
			int dontcare = 10;
		}
//		as = memory[read_pointer];
//		as = as/scale;
		outputtemp = memory[read_pointer] * twiddler(read_pointer);

		outputtemp = outputtemp / FixedComplex<32>(scale,0);
		output(outputtemp);

		if (read_pointer == ((N/2)-1))
		{

			state = FFFT_STATE_INITIAL;
			write_pointer = 0;
			read_pointer = 0;
		}
		else
		{
			read_pointer++;
		}

		break;
	}

	clock++;
	ready = (state != FFFT_STATE_OUTPUT);

}




fixedfftprint::fixedfftprint(int Ninput)
{
	N = Ninput;
	count = 0;
	ready = 1;
}

void fixedfftprint::inputandtick(FixedComplex<32> x)
{
//	sc_int<32> a = pow(2,15);
//	x = x * a;
	cout << "output[" << count << "]: " << x.real.to_int()/32.0 << "," << x.imag.to_int()/32.0 << " " <<  x << endl;
	count++;
}

fixedfftbuffer::fixedfftbuffer(int Ninput)
{
	N = Ninput;
	count = 0;
	ready = 1;
}

void fixedfftbuffer::inputandtick(FixedComplex<32> x)
{
	buf.push_back(x);
	count++;
}





fixedfft::fixedfft(int Ninput):printer(Ninput){
	N = Ninput;
	stagecount = log2(N);
	int stagesize = 0;

	stages = new fixedfftstage[stagecount];

	int i;
	for(i = 0; i < stagecount; i++)
	{
		stagesize = pow(2,(stagecount-i));
		stages[i].init(stagesize);

		if(i>0)
		{
			stages[i-1].next = &stages[i];
		}
	}

	stages[stagecount-1].next = &printer;
}

void fixedfft::inputandtick(FixedComplex<32> x)
{
//	cout << "input was " << x.real << endl;
//	sc_int<32> a = pow(2,15);
//	x = x / a;
	while(!stages[0].ready)
	{
		stages[0].inputandtick(FixedComplex<32>(0,0));
	}
	stages[0].inputandtick(x);
}






