#include "osiderived.h"

using namespace std;

OsiDerived::OsiDerived(int dat) : OsiBase(dat)
{

}//OsiDerived constructor


void OsiDerived::txdown(int* data, bool meta)
{
   // return down->rxup(data, meta);
}//Transmit to lower layer

void OsiDerived::txup(int* data, bool meta)
{
//    return up->rxdown(data,meta);
}//transmit to upper layer

 void OsiDerived::rxdown(int* data, bool meta)
{
        cout << "Warning." << name << " does not implement rxdown().";
   //     return this->txup(data,meta);
}//Receive from the lower layer  VIRTUAL


 void OsiDerived::rxup(int* data, bool meta)
{
    cout << "Warning. " << name << " does not implement rxup().";
 //   return this->txdown(data,meta);
}//Recieve from the up layer  VIRTUAL

void OsiDerived::tick()
{}//Pass

    // void set_parent(OsiDerived* parent)
    // {
    //     this->up = parent;
    //     *parent->down = this;
    // }; //Sets the parent


OsiDerived::~OsiDerived()
{

}
