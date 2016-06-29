#include "fixediir.h"
#include <iostream>
using namespace std;


double a[3] = {1, -1.33012033759563986, 0.644119155523180109}; // a coefficients
double b[3] = {0.087936161148626110,  0.138126495630287949, 0.087936161148626110};//b coefficients

int main()
{

	double output[1024];
	double input[1024];
	for (int i = 1; i < 7; i++)
	{
		input[i-1] = (double)i/10.00;
	}

	fixediir iir(3, a, b);
	iir.iir(input,output,6);


	for (int i = 0; i < 6; i ++)
		cout << i+1 << ": " << output[i] << endl;
	return 0;
}

