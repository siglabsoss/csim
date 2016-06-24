
//  Connects REQ socket to tcp://localhost:5555


#include "plotter.cpp"
#include <iostream>
using namespace std;
int main ()
{
	plotter* plot = new plotter();
	vector<complex<double> > cvector;
	for (int i = 0; i < 100; i++)
	{
			complex<double> c(i, i*3);
			cvector.push_back(c);
	}
//
//	 for (vector<complex<double> >::iterator it = cvector.begin() ; it != cvector.end(); ++it)
//	    std::cout << ' ' << *it;

	 plot->nplotqam(cvector, "title");


	vector<int> myVector;

	for (int i = 0; i < 100; i++)
		myVector.push_back(i * (i-1));


	plot->nplot(myVector,"heyo");

    return 0;
}
