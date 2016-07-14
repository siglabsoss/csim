#include "Stitcher.h"

int main()
{
	Stitcher stitch;
	int waves[3] = {0,1,2};
	int percent[3] = {20,30,50};

	stitch.format(waves, percent);
	stitch.stitch();
}
