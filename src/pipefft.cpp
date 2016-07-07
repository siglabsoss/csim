#include <iostream>

using namespace std;

#include "floatfft.h"

int main(void)
{
	int i;
	cout << "program start" << endl;

	floatfft fft(8);
	for(i = 0; i < 10; i++)
	{
		fft.inputandtick(5);
		fft.inputandtick(6);
		fft.inputandtick(8);
		fft.inputandtick(-5);
		fft.inputandtick(6);
		fft.inputandtick(12);
		fft.inputandtick(10);
		fft.inputandtick(9);
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

