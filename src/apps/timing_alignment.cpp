#include <utils/utils.hpp>
#include <types/fixedcomplex.hpp>
#include <iostream>

using namespace std;



//static void runFilter(const std::vector<ComplexDouble>& inputs)
//
//{
//        printf("Running filter with %ld input samples\n", inputs.size());
//
//    for (unsigned i = 0; i < inputs.size(); ++i) {
//        printf("%f,%f\n",
//               inputs[i].real(), inputs[i].imag());
//    }
//
//    // Write filter here
//
//
//   zadoff=zadoff_chu(Ns,u);//generate the zadoff-Chu sequence
//   z=convolve(input.real,input.imag,zadoff.real, zadoff.imag);//convolve the input signal with Zadoff chu
//
//}
//
static void zadoff_chu(unsigned length, int root, double *zc_sequence)
{
	unsigned i;
	double argument;
	double pi;

	pi = 4 * atan(1.0);

	if ((length & 0x1) != 0) {
		// Odd Length Sequence
		for (i = 0; i < length; i++) {
			argument = -pi * root * i * (i + 1);
			argument /= length;
			zc_sequence[2*i] = cos(argument);
			zc_sequence[2*i+1] = sin(argument);
		}
	}
	else {
		// Even Length Sequence
		for (i = 0; i < length; i++) {
			argument = -pi * root * i * i;
			argument /= length;
			zc_sequence[2*i] = cos(argument);
			zc_sequence[2*i+1] = sin(argument);
		}
	}
}
//
static void build_pilot(unsigned Nfft, unsigned Ncp, unsigned Na, int first_tone, int *pilot)
{
	//memset(pilot, 0, Nfft+Ncp);
	unsigned i= int(Nfft/2-first_tone);
	pilot[i]=1;
	while  (i>=1)
	{
		int j=2*i;
		pilot[j]=1;
		pilot[j+1]=0;
		i-=2*(Nfft/2-first_tone)/Na;
	}
	i=Nfft/2+first_tone;
	while  (i<=Nfft-1)
		{
			unsigned j=2*i;
			pilot[j]=1;
			pilot[j+1]=0;
			i+=2*(Nfft/2-first_tone)/Na;
		}
//****************************call FFT to turn pilot in to time domain

}


//
//convolution algorithm
static void convolve(int *A, double *B, unsigned len_A, unsigned len_B, int *C)
{

	//allocated convolution array
//	C = (int*) calloc(lenA+lenB-1, sizeof(float));

	//convolution process
	for (unsigned i=0; i<=len_A+len_B-1; i+=2)
	{
		double tempR=0.5;
		double tempI=0.5;
		unsigned j=0;
		while (j<len_B && j<=i)
		{
				tempR+=(double)A[j]*B[i-j]-(double)A[j+1]*B[i-j+1];
				tempI+=(double)A[j+1]*B[i-j]-(double)A[j]*B[i-j+1];
				j+=2;
		}
		//saturate and round
		if ( tempR > 32767.0 )
			tempR = 32767.0;
		else if ( tempR < -32768.0 )
			tempR = -32768.0;
		if ( tempI > 32767.0 )
			tempI = 32767.0;
		else if ( tempI < -32768.0 )
			tempI = -32768.0;

		C[i]= (int) tempR;
		C[i+1]=(int) tempI;
	}
}
//}
//
//
//
//

int main(int argc, char *argv[])
{
//	int  c;
//    bool gotInputFile = false;
    unsigned Nfft=1024;
    unsigned Ncp = int(Nfft/8);
    unsigned Na = 16;
    int first_tone=128;
    unsigned Ns =29;
    unsigned u=3; //root of Zadoff-Chu sequence
//    double CFO=800;
//    double SCO=0;//400e-9;%400 PPB
//    int z_real[inputs.size()]
    double input[10]={1,1,2,2,3,3,4,4,5,5};
	unsigned input_len=sizeof(input)/2/sizeof(double);
	unsigned zc_len = 2*Ns;
    double zc_seq[zc_len] ;
    unsigned pilot_len=2*Nfft;
	int pilot[pilot_len]= {0};
	int output[pilot_len+input_len-1]={0};


//
//    std::string inputFile;
//
//    while ((c = getopt(argc, argv, "f:")) != -1) {
//        switch (c) {
//        case 'f':
//            inputFile    = std::string(optarg);
//            gotInputFile = true;
//            break;
//        }
//    }
//
//    if (gotInputFile != true) {
//        std::cout << "Usage: " << argv[0] << " -f <input filename>" << std::endl;
//        return 1;
//    }
//
//    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(
//        inputFile);
//
//    assert(inputs.size() > 0);
    zadoff_chu(Ns,u,zc_seq);
    build_pilot(Nfft, Ncp, Na, first_tone, pilot);//build the pilot
    convolve(pilot,input, pilot_len,input_len,output);//convolve the pilot with the input signal
//    runFilter(inputs);

    unsigned i;
    for(i=0;i<zc_len-1;i+=2){
    	cout << zc_seq[i] << "," << zc_seq[i+1] << endl;
//    	printf("%x,%x\n", (int)zc_seq[i], (int)zc_seq[i+1]);
    }
//


    return 0;
} // main
