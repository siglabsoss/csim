#include "Stitcher.h"

int main()
{
	int waves[4] = {1,0,1,0};
	int samples[4] = {100,100,100,100};
//	int waves[2] = {1,1};
//	int samples[2] = {50,50};
	int num_sections = sizeof(samples) / sizeof(samples[0]);
	vector<FixedComplex<32> > datas;
	for (int k = 0; k < 7 ; k++)
		for ( int i = 0; i < 32768; i++)
		{
			FixedComplex<32> j(i,i);
		//	cout << j;
			datas.push_back(j);
		}

	Stitcher stitch(waves, samples, num_sections);
	vector<FixedComplex<32> > outdatas;
	outdatas = stitch.stitch(400, 1000, 100, datas);//(int numsamples, int sampleRate, int frequency);

	int size = outdatas.size();
	string outFile = "data/output/stitching2.csv";
	ofstream out(outFile.c_str());

	for (int i = 0; i < size; i++)
	{
		out << outdatas[i].real/32768.0 <<"," << outdatas[i].imag/32768.0 << endl;
		cout << outdatas[i];
	}
	cout << "DONE"<< endl;
}
