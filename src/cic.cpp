#include "cic.h"

using namespace std;

fixedcic::fixedcic(int R,int aregs, int bregs)
{
	this->r = R;
	numBRegisters = bregs;
	numARegisters = aregs;
	a = new FixedComplex<32>[aregs];
	b = new FixedComplex<32>[bregs];
	samples =  0;
}

int fixedcic::cic(int length, FixedComplex<16>* input, FixedComplex<16>* output)
{
	samples = 0; //reset sample count
	for (int i = 0; i < this->numBRegisters; i++)
	{
		b[i].real = 0;
		b[i].imag = 0;

	}//Initialize registers

	for (int i = 0; i < this->numARegisters; i++)
	{
		a[i].real = 0;
		a[i].imag = 0;
	}//Initialize registers

	FixedComplex<32> temp; //Storage for integrate output
	int k = 0;
	for(int j = 0; j < length; j++)
	{
		temp = integrate(input[j]); //Calculate filtered data
		if (!(this->downsample())) //If not downsampled
			output[k++]= ((this->comb(temp))).to_16(); //converts final value of comb to 16 bits.
	}

	return k; //returns number of outputs
}




FixedComplex<32> fixedcic::integrate(FixedComplex<16> current)
{
	 FixedComplex<32> CenterTap;

	 CenterTap = current.to_32();

	 for (int i = 0; i < numBRegisters; i++)
	 {
		 CenterTap = CenterTap + b[i].to_32();//Accumulate for each b register
		 b[i] = CenterTap;
	 }

	 return CenterTap;
}//Performs filtering



FixedComplex<32> fixedcic::comb(FixedComplex<32> current)
{
	 FixedComplex<32> final;
	 FixedComplex<32> temp;//storage for swap

	 final = current.to_32();

	 for (int i = 0; i < numARegisters; i++)
	 {
		 temp = final;
		 final = final - a[i].to_32();//Accumulate for each b register
		 a[i] = temp;
	 }

	 return final;
}//Performs


bool fixedcic::downsample()
{
	if ((samples % r) == 0)//if time to take in a new sample
	{
		samples++;
		return false;
	}
	else//otherwise do not comb
	{
		samples++;
		return true;
	}
}


fixedcic::~fixedcic()
{

}

