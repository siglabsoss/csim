#include <iostream>

using namespace std;

#include "fixedfft.h"
#include "floatfft.h"

int main(void)
{





	cout << "program start" << endl;



	int i;

//	{
//	floatfft fft(8);
//	for(i = 0; i < 3; i++)
//	{
//		fft.inputandtick(5);
//		fft.inputandtick(6);
//		fft.inputandtick(8);
//		fft.inputandtick(-5);
//		fft.inputandtick(6);
//		fft.inputandtick(12);
//		fft.inputandtick(10);
//		fft.inputandtick(9);
//	}
//	}



//	cout << "---------------------------------------------------" << endl;
//	cout << "---------------------------------------------------" << endl;


	int a = pow(2,15);
	int scale = a/1000.0;


	fixedfft fft(8);
	for(i = 0; i < 3; i++)
	{

		fft.inputandtick(FixedComplex<32>(5 * scale,0));
		fft.inputandtick(FixedComplex<32>(6 * scale,0));
		fft.inputandtick(FixedComplex<32>(8 * scale,0));
		fft.inputandtick(FixedComplex<32>(-5 * scale,0));
		fft.inputandtick(FixedComplex<32>(6 * scale,0));
		fft.inputandtick(FixedComplex<32>(12 * scale,0));
		fft.inputandtick(FixedComplex<32>(10 * scale,0));
		fft.inputandtick(FixedComplex<32>(9 * scale,0));
	}


	//
	//	floatfftstage stageone(8);
	//	floatfftstage stagetwo(4);
	//	floatfftstage stagethree(2);
	//	floatfftprint printer(8);
	//
	//	stageone.next = &stagetwo;
	//	stagetwo.next = &stagethree;
	//	stagethree.next = &printer;
	//	for(i = 0; i < 10; i++)
	//	{
	//		stageone.inputandtick(5);
	//		stageone.inputandtick(6);
	//		stageone.inputandtick(8);
	//		stageone.inputandtick(-5);
	//		stageone.inputandtick(6);
	//		stageone.inputandtick(12);
	//		stageone.inputandtick(10);
	//		stageone.inputandtick(9);
	//	}






	//	floatfftstage stageone(4);
	//	floatfftstage stagetwo(2);
	//	floatfftprint printer(8);
	//
	//	stageone.next = &stagetwo;
	//	stagetwo.next = &printer;
	//
	//	stageone.inputandtick(5);
	//	stageone.inputandtick(6);
	//	stageone.inputandtick(8);
	//	stageone.inputandtick(-5);
	//
	//
	//	stageone.inputandtick(5);
	//	stageone.inputandtick(6);
	//	stageone.inputandtick(8);
	//	stageone.inputandtick(-5);
	//
	//	stageone.inputandtick(5);
	//	stageone.inputandtick(6);
	//	stageone.inputandtick(8);
	//	stageone.inputandtick(-5);





	cout << "program end" << endl;

}

