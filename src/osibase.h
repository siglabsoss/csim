#include <iostream>
#include <string>
#include <cstddef>
#include "systemc.h"
using namespace std;

typedef sc_uint<4> foobar;
class OsiBase
{
private:

protected:
	OsiBase* up;
	OsiBase* down;
	string name;
	int data;

public:

	OsiBase(int dat);
    void txdown(int* data, unsigned size, bool meta);
    void txup(int* data, unsigned size,bool meta);
    virtual void rxdown( vector<foobar> data, unsigned size,bool meta);
    virtual void rxup(vector<foobar> data, unsigned size,bool meta);
    void tick();
    void setParent(OsiBase* parent);
    virtual ~OsiBase();

};
