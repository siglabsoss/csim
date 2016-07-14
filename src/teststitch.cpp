#include "Stitcher.h"

int main()
{
	int waves[5] = {1,2,1,0,1};
	int percent[5] = {10,10,20,60};
	int num = 5;
	vector<float> datas;
	for (float i = 0; i < 10; i++)
	{
		datas.push_back(i);
	}
	Stitcher stitch(waves, percent, num,datas);

	datas = stitch.stitch(100, 10,2);//(int numsamples, int sampleRate, int frequency);

	int size = datas.size();
	string outFile = "data/output/stitching.csv";
	ofstream out(outFile.c_str());

	for (int i = 0; i < size; i++)
		out << datas[i] << endl;

	cout << "DONE"<< endl;
}
