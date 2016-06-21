#include <iostream>
#include <string>
#include <cstddef>
using namespace std;

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
    virtual void rxdown(int* data, unsigned size,bool meta);
    virtual void rxup(int* data, unsigned size,bool meta);
    void tick();
    void setParent(OsiBase* parent);
    virtual ~OsiBase();

};

