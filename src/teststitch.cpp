#include "Stitcher.h"

int main()
{
	int waves[4] = {1,1,1,1};
	int percent[4] = {25,25,25,25};
	int num = 4;
	vector<FixedComplex<32> > datas;
	for (float i = 0; i < 32768; i++)
	{
		FixedComplex<32> j(i,i);
		cout << j;
		datas.push_back(j);
	}

	Stitcher stitch(waves, percent, num,datas);

	datas = stitch.stitch(1000, 100,1);//(int numsamples, int sampleRate, int frequency);

	int size = datas.size();
	string outFile = "data/output/stitching.csv";
	ofstream out(outFile.c_str());

	for (int i = 0; i < size; i++)
	{
		out << datas[i].real/32768.0 <<"," << datas[i].imag/32768.0 << endl;
		cout << datas[i];
	}
	cout << "DONE"<< endl;
}
