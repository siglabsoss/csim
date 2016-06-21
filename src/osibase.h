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
    void txdown(int* data, bool meta);
    void txup(int* data, bool meta);
    virtual void rxdown(int* data, bool meta);
    virtual void rxup(int* data, bool meta);
    void tick();
    virtual ~OsiBase();

};

