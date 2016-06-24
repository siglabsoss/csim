
//  Connects REQ socket to tcp://localhost:5555

#include <zmq.hpp>
#include <string>
#include <iostream>
#include "plotter.cpp"
#include <cstring>
#include <string>

int main ()
{
	vector<int>* myVector = (new vector<int>);

	for (int i = 0; i < 100; i++)
		myVector->push_back(i * (i-1));

	plotter* plot = new plotter();
	plot->nplot(*myVector,"heyo");




    return 0;
}
