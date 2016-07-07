#include <iostream>

using namespace std;

#include "floatfft.h"

int main(void)
{

	cout << "program start" << endl;


	floatfftstage stageone(8);
	floatfftstage stagetwo(4);
	floatfftstage stagethree(2);
	floatfftprint printer(8);


	stageone.next = &stagetwo;
	stagetwo.next = &stagethree;
	stagethree.next = &printer;

	//	stageone.next = &stagetwo;
	//	stagetwo.next = &printer;


	stageone.inputandtick(5);
	stageone.inputandtick(6);
	stageone.inputandtick(8);
	stageone.inputandtick(-5);
	stageone.inputandtick(6);
	stageone.inputandtick(12);
	stageone.inputandtick(10);
	stageone.inputandtick(9);

	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);

	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);
	stageone.inputandtick(0);

	stageone.inputandtick(0);
	stageone.inputandtick(0);




	cout << "program end" << endl;

}

