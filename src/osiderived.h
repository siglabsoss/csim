#include <iostream>
#include <string>
#include <cstddef>
#include "osibase.h"
#include "systemc.h"
typedef sc_uint<4> foobar;
using namespace std;

class OsiDerived : public OsiBase
{
private:
	sc_uint<4> factor;


public:


	OsiDerived(int dat);
    void txdown(int* data, unsigned size, bool meta);
    void txup(int* data, unsigned size, bool meta);
    virtual void rxdown(vector<foobar> data, unsigned size, bool meta);
    virtual void rxup(vector<foobar> data, unsigned size, bool meta);
    void tick();
    virtual ~OsiDerived();


};
