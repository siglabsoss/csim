
#include "osibase.h"
using namespace std;

OsiBase::OsiBase(int dat)
{
        this->up = 0;
        this->down = 0;
        this->name = "noname";
        this->data = dat;
}//OsiBase constructor

  
void OsiBase::txdown(int* data, unsigned size, bool meta)
{
   // return down->rxup(data, meta);
}//Transmit to lower layer

void OsiBase::txup(int* data, unsigned size, bool meta)
{
//    return up->rxdown(data,meta);
}//transmit to upper layer

 void OsiBase::rxdown(int* data, unsigned size, bool meta)
{
        cout << "Warning." << name << " does not implement rxdown().";
   //     return this->txup(data,meta);
}//Receive from the lower layer  VIRTUAL


 void OsiBase::rxup(int* data, unsigned size, bool meta)
{
    cout << "Warning. " << name << " does not implement rxup().";
 //   return this->txdown(data,meta);
}//Recieve from the up layer  VIRTUAL

void OsiBase::tick()
{

}//Pass

    // void set_parent(OsiBase* parent)
    // {
    //     this->up = parent;
    //     *parent->down = this;
    // }; //Sets the parent


OsiBase::~OsiBase()
{

}
