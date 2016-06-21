#include <iostream>
//#include "osibase.h"
#include "osiderived.h"
using namespace std;

int main()
{
    cout << "Hello\n";
    OsiBase* lowerBase = new OsiDerived(1);
    OsiBase* upperBase = new OsiBase(1);
    int* x = 0;
    lowerBase->txdown(x, false);
    return 0;
}


