#include "Stitcher.h"

int main()
{
	int waves[5] = {1,2,1,0,1};
	int percent[5] = {10,10,20,60};
	int num = 5;

	Stitcher stitch(waves, percent, num);

	stitch.stitch(100, 10,2);//(int numsamples, int sampleRate, int frequency);
	cout << "DONE"<< endl;
}
