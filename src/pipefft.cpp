#include <iostream>

using namespace std;

#include "floatfft.h"

int main(void)
{

	cout << "program start" << endl;


	floatfft stageone(8);

	stageone.memory[3] = 2343;

	stageone.dump();


	cout << "program end" << endl;

}

