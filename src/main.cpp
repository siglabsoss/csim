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
    int data[10] = {0,1,2,3,4,5,6,7,8,9};

    lowerBase->txdown(data, 10, false);
    return 0;
}


