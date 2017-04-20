#include <utils/utils.hpp>
#include <types/fixedcomplex.hpp>
#include <iostream>
#include <vector>
#include <filters/fft.hpp>
#include <types/fixedpoint.hpp>
#include <cmath>


using namespace std;



//static void runFilter(const std::vector<Compleatan2xDouble>& inputs)
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
static void complex_convolve(std::vector<SLFixComplex> &A,std::vector<SLFixComplex> &B, unsigned len_A , unsigned len_B, std::vector<SLFixComplex> &C)
{
	for (unsigned i=0; i<len_A+len_B-1; ++i)
		{

			unsigned j;
			j = (i>len_A-1)?i-(len_A-1):0;
			while (j<len_B && j<=i)
			{
				C[i]+=B[j]*A[i-j];
				if (C[i].real().to_double()>1.99|| C[i].imag().to_double()>1.99 || C[i].real().to_double()<-1.99|| C[i].imag().to_double()<-1.99){
						cout<<"saturated"<<endl;
				}
				j+=1;
			}
		}
}

static double CFO_Moose(std::vector<SLFixComplex> &A,std::vector<SLFixComplex> &B, unsigned len, double Fs){
	std::vector<SLFixComplex> FFTA(len, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	std::vector<SLFixComplex> FFTB(len, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	SLFixComplex dotprod(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE);

	 FFTA=FFTWrap(A, false/* FFT */,18,1);
	 FFTB=FFTWrap(B, false/* FFT */,18,1);
	 for (unsigned i=0; i<len; ++i){
		  dotprod=dotprod+FFTA[i]*FFTB[i];
	 }

	 return std::atan2(dotprod.imag().to_double(),dotprod.real().to_double())/(2*M_PI*len/Fs);
}
//}
//}

int main(int argc, char *argv[])
{
//	int  c;
//    bool gotInputFile = false;
	constexpr unsigned Nfft=1024;
    constexpr unsigned Ncp = int(Nfft/8);
    constexpr unsigned num_sym=2;
    std::vector<double> input_temp(num_sym*2*(Nfft+Ncp));//{0.01,-0.03,0.19,-0.008,-0.00053,-0.034,-0.140,0.014,-0.050,0.0015};
	std::vector<SLFixComplex> input(num_sym*(Nfft+Ncp), SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	std::vector<SLFixComplex> dummyin(input.size(), SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	std::vector<SLFixComplex> dummyout(input.size()*2-1, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	std::vector<SLFixComplex> dumbo1(Nfft, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	std::vector<SLFixComplex> dumbo2(Nfft, SLFixComplex(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE));
	unsigned xcorr_out_len=2*input.size()-1;
	struct autocorr {
	      autocorr() :
	          data(18, 2, SLFixPoint::QUANT_RND_HALF_UP, SLFixPoint::OVERFLOW_SATURATE),
	          order(0)
	      {}

	      SLFixComplex data;
	      unsigned order;
	      //bool operator<( const autocorr &a, const autocorr &b)
	      bool operator<(const autocorr &rhs)
	      {
	          return std::abs(this->data.toComplexDouble()) < std::abs(rhs.data.toComplexDouble());
	      }
	};

	std::vector<autocorr> xcor_out(xcorr_out_len);
	std::vector<autocorr> copy_xcor(xcorr_out_len);

    //constexpr unsigned Na = 16;
    //constexpr int first_tone=128;
//    double input[10]={1.0,1.0,1.9,1.9,1.3,1.3,-1.40,-1.4,1.50,1.50};
//    unsigned input_len=sizeof(input)/sizeof(double);
//    for (unsigned i=0; i<input_len; ++i){
//    	input[i]/=1;
//    }
    //unsigned pilot_len=2*Nfft;
	//unsigned pilot_len=10;
    std::vector<double> CFO(xcorr_out_len/(Nfft+Ncp)/2);
    //double pilot[pilot_len]= {0};
    //double pilot[pilot_len]= {1.30,1.50,1.40,0.60,0.90,-1.00,1.10,-1.330,-1.420,1.20};
	//double output[pilot_len+input_len-1]={0};
	//declarations for the PN sequence
//	int8_t PNseq[2*Nfft]={0};
//	unsigned poly  = (1 << 11) | (1 << 9) | (1 << 0);
//	PNgenerator(11, 0x00001, poly, PNseq);
////
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
//xcor_out.d
//    assert(inputs.size() > 0);

    //}
    // test module
    ifstream inFile("testing.txt");
    ofstream outFile("ta.txt");
    unsigned i=0;
    while(!inFile.eof()) {
        inFile >> input_temp[i]>> input_temp[i+1];
//        input_temp[i]*= 1>>18;
//        input_temp[i+1]*= 1>>18;
//        cout<<input_temp[i]<<" "<<input_temp[i+1]<<endl;
        i+=2;
      }
    for (unsigned i = 0; i < input_temp.size()/2; ++i) {
    		input[i].set(input_temp[i*2], input_temp[i*2+1]);
//    		cout<<input[i].real()<<' '<<input[i].imag()<<endl;
    }
    for (unsigned i=0; i<input.size(); ++i){
    	dummyin[i]=*input[i];
//    	outFile << dummyin[i].real()<<" , "<<dummyin[i].imag() << endl;
    }
    std::reverse(dummyin.begin(), dummyin.end());
    for (unsigned i=0; i<dummyin.size(); i++){
//    	outFile << dummyin[i].real()<<" , "<<dummyin[i].imag() << endl;
    }
    complex_convolve(input,dummyin, input.size(),dummyin.size(),dummyout); //autocorr the input sequence
    for (unsigned i=0; i<xcor_out.size(); i++){
    	xcor_out[i].order=i;
    	copy_xcor[i].order=i;
    	xcor_out[i].data=dummyout[i];
    	copy_xcor[i].data=dummyout[i];
    	outFile << dummyout[i].real()<<" , "<<dummyout[i].imag() << endl;
    }

    std::sort(xcor_out.begin(),xcor_out.end());
    //************************ for now set it at 10
    for (unsigned i=0; i<input.size()/10; i+=2){
    	for (unsigned j=0; j<Nfft; ++j)
    	{
    		unsigned k=xcor_out[i].order;
    		dumbo1[j]=copy_xcor[k+j].data;
    		dumbo2[j]=copy_xcor[k+Nfft+Ncp+j].data;

    	}
    		CFO[i]=CFO_Moose(dumbo1,dumbo2,Nfft, 32e6);
    }
    cout << "CFO is "<< std::accumulate(CFO.begin(),CFO.end(),0)/(double)CFO.size()<<endl;
    //
//    unsigned j=0;
//    for (int index=(Nfft+Ncp)*(zc_len-1); index>=0;  index=index-(Nfft+Ncp)){
//    		s_up[index]=zc_seq[j];
//    		j++;
//    }
//    std::reverse(s_up.begin(), s_up.end());
//    complex_convolve(output, s_up, output.size(), s_up.size(), match_out);//convolve with s_up sequence
//    runFilter(inputs);
//    ofstream outFile("ta.txt");
//    outFile << "ta = [" << endl;
    return 0;
} // main
