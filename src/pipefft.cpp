#include <iostream>

using namespace std;

#include "floatfft.h"

int main(void)
{

	cout << "program start" << endl;


	floatfft stageone(8);

	stageone.inputandtick(5);
	stageone.inputandtick(6);
	stageone.inputandtick(8);
	stageone.inputandtick(-5);
	stageone.inputandtick(6);
	stageone.inputandtick(12);
	stageone.inputandtick(10);
	stageone.inputandtick(9);



	cout << "program end" << endl;

}

