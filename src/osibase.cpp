#include <iostream>
#include <string>
#include <cstddef>
using namespace std;

class OsiBase
{
private:
	OsiBase* up;
	OsiBase* down;
	string name;
  
public:
	OsiBase() 
    {
        this->up = 0;
        this->down = 0;
        this->name = "noname";
    };//OsiBase constructor

  
    void txdown(int* data, bool meta)
    {
       // return down->rxup(data, meta);
    };//Transmit to lower layer

    void txup(int* data, bool meta)
    {
    //    return up->rxdown(data,meta);
    };//transmit to upper layer

    virtual void rxdown(int* data, bool meta)
    {
        cout << "Warning." << name << " does not implement rxdown().";
   //     return this->txup(data,meta);
    };//Receive from the lower layer  VIRTUAL


    virtual void rxup(int* data, bool meta)
    {
        cout << "Warning. " << name << " does not implement rxup().";
     //   return this->txdown(data,meta);
    };//Recieve from the up layer  VIRTUAL

    void tick()
    {

    };//Pass

    // void set_parent(OsiBase* parent)
    // {
    //     this->up = parent;
    //     *parent->down = this;
    // }; //Sets the parent
};

