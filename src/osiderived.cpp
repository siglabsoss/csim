#include "osiderived.h"

using namespace std;

OsiDerived::OsiDerived(int dat) : OsiBase(dat)
{
	this->factor = 2;
}//OsiDerived constructor


void OsiDerived::txdown(int* data, unsigned size, bool meta)
{

}//Transmit to lower layer

void OsiDerived::txup(int* data, unsigned size, bool meta)
{

}//transmit to upper layer

void OsiDerived::rxdown(int* data, unsigned size, bool meta)
{
	 int dout[size];
	 for(unsigned i = 0; i < size; i++)
	 {
		 dout[i] = data[i] * this->factor;
	 }
	 this->up->rxdown(dout,size,meta);

}


 void OsiDerived::rxup(int* data, unsigned size, bool meta)
{
    cout << "Warning. " << name << " does not implement rxup().";

}//Recieve from the up layer  VIRTUAL

void OsiDerived::tick()
{}//Pass



OsiDerived::~OsiDerived()
{

}
