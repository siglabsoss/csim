#include <iostream>
#include "osiderived.h"
#include "systemc.h"
#include <vector>



typedef sc_uint<4> foobar;

using namespace std;

int main()
{

	vector<foobar> myvector;
	sc_uint<4> test[5] = {1,1,2,3,4};

	for (int i = 0; i < 5; i ++)
		myvector.push_back ( (test[i]) ); //inserts data in reverse order
    cout << "Hello\n";
    OsiDerived* lowerBase = new OsiDerived(1);
    OsiDerived* upperBase = new OsiDerived(1);
    OsiBase* topBase = new OsiBase(1);
    lowerBase->setParent(upperBase);
    upperBase->setParent(topBase);

    lowerBase->rxdown(myvector, 5, false);
    cout << "Original numbers:" << endl;
	for (int i = 0; i < 5; i++)
		cout << myvector[i] << endl; // prints data
    return 0;
}


