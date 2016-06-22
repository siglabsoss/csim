#include "osiderived.h"

using namespace std;

OsiDerived::OsiDerived(int dat) : OsiBase(dat)
{
	sc_uint<4> fact = 2;
	this->factor = fact;
}//OsiDerived constructor


void OsiDerived::txdown(int* data, unsigned size, bool meta)
{

}//Transmit to lower layer

void OsiDerived::txup(int* data, unsigned size, bool meta)
{

}//transmit to upper layer

void OsiDerived::rxdown(vector<foobar> data, unsigned size, bool meta)
{
	 vector<foobar> dout;
	 for (int i = 0; i < 5; i++)
		 dout.push_back(data[i] * factor);
	 this->up->rxdown(dout,size,meta);

}


 void OsiDerived::rxup(vector<foobar> data, unsigned size, bool meta)
{
    cout << "Warning.  " << name << " does not implement rxup(). " << endl;

}//Recieve from the up layer  VIRTUAL

void OsiDerived::tick()
{}//Pass



OsiDerived::~OsiDerived()
{

}
