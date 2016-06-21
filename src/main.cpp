#include <iostream>
#include "osiderived.h"
using namespace std;

int main()
{
    cout << "Hello\n";
    OsiBase* lowerBase = new OsiDerived(1);
    OsiBase* upperBase = new OsiDerived(1);
    OsiBase* topBase = new OsiBase(1);
    lowerBase->setParent(upperBase);
    upperBase->setParent(topBase);
    int data[10] = {0,1,2,3,4,5,6,7,8,9};
    lowerBase->rxdown(data, 10, false);
    return 0;
}


