#include <utils/utils.hpp>
#include <types/fixedcomplex.hpp>
#include <iostream>
#include <vector>
#include <filters/fft.hpp>
#include <types/fixedpoint.hpp>
#include <cmath>


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
static void zadoff_chu(unsigned length, int root, std::vector<SLFixComplex> &zc_sequence)
{
	unsigned i;
	double argument;
//	double pi;


	if ((length & 0x1) != 0) {
		// Odd Length Sequence
		for (i = 0; i < length; i++) {
			argument = -M_PI * root * i * (i + 1);
			argument /= length;
			zc_sequence[i].set(cos(argument),sin(argument));
		}
	}
	else {
		// Even Length Sequence
		for (i = 0; i < length; i++) {
			argument = -M_PI * root * i * i;
			argument /= length;
			zc_sequence[i].set(cos(argument),sin(argument));

		}
	}
}
//
static void build_pilot(unsigned Nfft, unsigned Ncp, unsigned Na, int first_tone, double *pilot, int8_t *PN_seq)
{
	unsigned m=0;
	unsigned i= int(Nfft/2-first_tone);
	pilot[i] = 1.0;
	while  (i>=1)
	{
		int j=2*i;
		pilot[j]=PN_seq[m++]/sqrt(Na);
		pilot[j+1]=0.0;
		i-=2*(Nfft/2-first_tone)/Na;
	}
	i=Nfft/2+first_tone;
	while  (i<=Nfft-1)
		{
			unsigned j=2*i;
			pilot[j]=PN_seq[m++]/sqrt(Na);
			pilot[j+1]=0.0;
			i+=2*(Nfft/2-first_tone)/Na;
		}

	std::vector<SLFixComplex> pilot_vector(1024, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	for (unsigned i = 0; i < pilot_vector.size(); ++i) {
		//pilotVector[i] = ComplexDouble(pilot[i*2], pilot[i*2+1]);
		pilot_vector[i].set(pilot[i*2], pilot[i*2+1]);
	}
	std::vector<SLFixComplex> output = FFTWrap(pilot_vector, true/* IFFT */,18, 1);
	for (unsigned i = 0; i < output.size(); ++i) {
		pilot[i*2]     = output[i].real().to_double();
		pilot[i*2 + 1] = output[i].imag().to_double();
	}

//	ofstream outFile("tafft.txt");
//		for (unsigned i=0; i<Nfft; ++i){
//			outFile<<pilot[2*i]<<","<<pilot[2*i+1]<<endl;
//		}

}


//
//convolution algorithm
static void matched_filter(double *A, double *B, unsigned len_A, unsigned len_B, std::vector<SLFixComplex> &C)
{



	//convolution process
		std::vector<SLFixComplex> tempA(len_A/2, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
		std::vector<SLFixComplex> tempB(len_B/2, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
		unsigned j;

		//conjugate and reverse i.e. A(n)=conj(A(-n));

		for (unsigned i = 0; i < len_A/2 ; ++i) {
				tempA[i].set(A[i*2], -1*A[i*2+1]);
			}
		std::reverse(tempA.begin(), tempA.end());
		for (unsigned i = 0; i < len_B/2 ; ++i) {
				tempB[i].set(B[i*2], B[i*2+1]);
			}
		//for (unsigned i=0; i<len_A/2; ++i){
			//cout<< tempA[i].real().to_double() << "," << tempA[i].imag().to_double() << endl;
		//}
	for (unsigned i=0; i<len_A/2+len_B/2-1; ++i)//i+=2)
	{

		j=(i>len_A/2-1)?i-(len_A/2-1):0;
		while (j<len_B/2 && j<=i)
		{

			C[i]+=tempB[j]*tempA[i-j];
			j+=1;
		}
	}
}
static void complex_convolve(std::vector<SLFixComplex> &A,std::vector<SLFixComplex> &B, unsigned len_A , unsigned len_B, std::vector<SLFixComplex> &C)//convolve with s_up sequence
{
	for (unsigned i=0; i<len_A+len_B-1; ++i)
		{
			if(i > 34430)
			{
				cout << i << endl;
			}
			unsigned j;
			j = (i>len_A-1)?i-(len_A-1):0;
			while (j<len_B && j<=i)
			{
				C[i]+=B[j]*A[i-j];
				j+=1;
			}
		}
}


static void PNgenerator(unsigned degree, unsigned seed, unsigned poly, int8_t *seq){
	typedef struct {
		unsigned state;
		unsigned mask;
		} Mseq;
	Mseq mseq;
	mseq.state=seed;
	mseq.mask  = poly >> 1;
	unsigned length=(1<<degree)-1;
	for (unsigned i=0;i<length ;++i){
		seq[i] = mseq.state & 1;
		mseq.state >>= 1;

		if ( seq[i] ){
			mseq.state ^= mseq.mask;
		}
		else{
			seq[i]=-1;
		}
		//outFile << (int)seq[i]<<" " << endl;
	}
}
//
//
//}
//}


//
//

int main(int argc, char *argv[])
{
//	int  c;
//    bool gotInputFile = false;
    constexpr unsigned Nfft=1024;
    constexpr unsigned Ncp = int(Nfft/8);
    constexpr unsigned Na = 16;
    constexpr int first_tone=128;
    constexpr unsigned Ns =29;
    constexpr unsigned u=3; //root of Zadoff-Chu sequence
//    int z_real[inputs.size()]
    double input[10]={1.0,1.0,1.9,1.9,1.3,1.3,-1.40,-1.4,1.50,1.50};
    unsigned input_len=sizeof(input)/sizeof(double);
    for (unsigned i=0; i<input_len; ++i){
    	input[i]/=1;
    }
	constexpr unsigned zc_len = Ns;
	std::vector<SLFixComplex> zc_seq(zc_len, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
    unsigned pilot_len=2*Nfft;
	//unsigned pilot_len=10;
    double pilot[pilot_len]= {0};
    //double pilot[pilot_len]= {1.30,1.50,1.40,0.60,0.90,-1.00,1.10,-1.330,-1.420,1.20};
    std::vector<SLFixComplex> output(pilot_len/2+input_len/2-1, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	//double output[pilot_len+input_len-1]={0};
	std::vector<SLFixComplex> s_up((Ncp+Nfft)*zc_len, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	std::vector<SLFixComplex> match_out(output.size()+s_up.size()-1, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	//declarations for the PN sequence
	int8_t PNseq[2*Nfft]={0};
	unsigned poly  = (1 << 11) | (1 << 9) | (1 << 0);
	PNgenerator(11, 0x00001, poly, PNseq);
//
//    std::string inputFile;
//the
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
//        return 1outFile << output[i].real() << "," << output[i].imag() << endl;;
//    }
//
//    std::vector<ComplexDouble> inputs = utils::readComplexFromCSV<ComplexDouble>(
//        inputFile);
//
//    assert(inputs.size() > 0);
    zadoff_chu(Ns,u,zc_seq);
    build_pilot(Nfft, Ncp, Na, first_tone, pilot,PNseq);//build the pilot
    matched_filter(pilot,input, pilot_len,input_len,output);//convolve the pilot with the input signal
    for(unsigned i=0;i<output.size();i++){
        	cout << output[i].real().to_double() << "," << output[i].imag().to_double() << endl;
            }
    unsigned j=0;
    for (int index=(Nfft+Ncp)*(zc_len-1); index>=0;  index=index-(Nfft+Ncp)){
    		s_up[index]=zc_seq[j];
    		j++;
    }
    std::reverse(s_up.begin(), s_up.end());
    complex_convolve(output, s_up, output.size(), s_up.size(), match_out);//convolve with s_up sequence
//    runFilter(inputs);
    ofstream outFile("ta.txt");

    outFile << "ta = [" << endl;

    for(unsigned i=0;i<zc_len-1;i++){
    	outFile << zc_seq[i].real().to_double() << "," << zc_seq[i].imag().to_double() << ';' << endl;
    }
    outFile << "];" << endl;
    outFile<<"pilot=["<<endl;
    for(unsigned i=0;i<pilot_len-1;i+=2){
        	outFile << pilot[i] << "," << pilot[i+1] << endl;
    }
    outFile << "];" << endl;


    outFile<<"output=["<<endl;
    for(unsigned i=0;i<output.size();i++){
    	outFile << output[i].real() << "," << output[i].imag() << endl;
        }
    outFile << "];" << endl;
    outFile<<"match_out=["<<endl;
    for(unsigned i=0;i<match_out.size();i++){
    	outFile << match_out[i].real()<<","<< match_out[i].imag()<< endl;
           }
    outFile << "];" << endl;
        //
    return 0;
} // main
