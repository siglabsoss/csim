#include "cic.h"

using namespace std;

fixedcic::fixedcic(int R)
{
	this->r = R;
	numBRegisters = 2;
	numARegisters = 2;
	a = new FixedComplex<32>[2];
	b = new FixedComplex<32>[2];
	sample =  0;
}

void fixedcic::cic(int length, FixedComplex<16>* input, FixedComplex<16>* output)
{
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

	FixedComplex<32> temp;
	for(int j = 0; j < length; j++)
	{
		temp = integrate(input[j]); //Calculate filtered data
		if (!(this->downsample()))
		{
		//	cout << temp;
			output[j]= ((this->comb(temp))).to_16();

		}
	}
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

	 //CenterTap = CenterTap >> 15;//Reset to 16 bits


	 return CenterTap;
}//Performs filtering



FixedComplex<32> fixedcic::comb(FixedComplex<32> current)
{
	 FixedComplex<32> final;

	 final = current.to_32();

	 for (int i = 0; i < numARegisters; i++)
	 {
		 final= final+ a[i].to_32();//Accumulate for each b register
		 a[i] = final;
	 }



	 return final;
}//Performs


bool fixedcic::downsample()
{
	if ((sample % r) == 0)
	{
		sample++;
		return false;
	}
	else
	{
		sample++;
		return true;
	}
}


fixedcic::~fixedcic()
{

}

