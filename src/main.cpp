#include <iostream>
#include "osibase.cpp"
using namespace std;

int main()
{
    cout << "Hello\n";
    OsiBase* lowerBase = new OsiBase;
    OsiBase* upperBase = new OsiBase;
    int* x = 0;
    lowerBase->txdown(x, false);
    return 0;
}