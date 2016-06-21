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
    void txdown(int* data, unsigned size, bool meta);
    void txup(int* data, unsigned size, bool meta);
    virtual void rxdown(int* data, unsigned size, bool meta);
    virtual void rxup(int* data, unsigned size, bool meta);
    void tick();
    virtual ~OsiDerived();

};
