#include <iostream>
#include <string>
#include <cstddef>
#include "osibase.h"
using namespace std;

class OsiDerived : public OsiBase
{
private:

public:
	OsiDerived(int dat);
    void txdown(int* data, bool meta);
    void txup(int* data, bool meta);
    virtual void rxdown(int* data, bool meta);
    virtual void rxup(int* data, bool meta);
    void tick();
    virtual ~OsiDerived();

};
