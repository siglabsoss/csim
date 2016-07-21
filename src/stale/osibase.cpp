#include "osibase.h"
using namespace std;

OsiBase::OsiBase(int dat)
{
    this->up = 0;
    this->down = 0;
    this->name = "noname";
    this->data = dat;
} //OsiBase constructor

void OsiBase::txdown(int* data, unsigned size, bool meta)
{

} //Transmit to lower layer

void OsiBase::txup(int* data, unsigned size, bool meta)
{

} //transmit to upper layer

void OsiBase::rxdown(vector<foobar> data, unsigned size, bool meta)
{
    cout << "Warning. " << name << " does not implement rxdown()" << endl;
    for (int i = 0; i < 5; i++)
        cout << data[i] << endl; // prints data
} //Receive from the lower layer  VIRTUAL

void OsiBase::rxup(vector<foobar> data, unsigned size, bool meta)
{
    cout << "Warning.  " << name << " does not implement rxup()." << endl;

} //Recieve from the up layer  VIRTUAL

void OsiBase::tick()
{

} //Pass

void OsiBase::setParent(OsiBase* parent)
{
    this->up = parent;
    parent->down = this;
}
;
//Sets the parent

OsiBase::~OsiBase()
{

}
