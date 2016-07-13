#include "cordic.h"
#include <iostream>



using namespace std;

int main()
{
	cordic c;
	long double i;
	cin >> i;
	FixedComplex<16> a(1,0);
	FixedComplex<16> b(0,0);
	c.calculate(i,a,b);


}
