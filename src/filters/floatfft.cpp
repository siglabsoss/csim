#include <iostream>
#include <cmath>


#include <fstream>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <iostream>     // cout, endl
#include <vector> //For storing parsed data from file
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp> //For parsing data from file
#include <iomanip> //For setprecision
#include <stdlib.h>
#include <fstream>
#include <utils/utils.hpp> //reverseBits()
#include <filters/floatfft.hpp>
using namespace std;

#include <filters/floatfft.hpp>

const float pi = 3.14159265359;

string outfile("../csim/data/fft/output/compareTwiddler.txt");
ofstream out3(outfile.c_str());

void floatfftstage::init(int Ninput)
{
    N = Ninput;

    memory = new complex<float> [N / 2];

    state = FFT_STATE_INITIAL;

    read_pointer = 0;
    write_pointer = 0;

    ready = 1;

    clock = 0;

//	cout << "FFT bufferfly " << N << " was created" << endl;
}

floatfftstage::floatfftstage(int Ninput)
{
    init(Ninput);
}

floatfftstage::floatfftstage()
{

}

void floatfftstage::dump(void)
{
    int i;
    for (i = 0; i < N / 2; i++) {
        cout << "Memory " << i << ": " << memory[i] << endl;
    }
}

void floatfftstage::butterfly(complex<float> array[2], complex<float> x,
        complex<float> y)
{
    array[0] = x + y;
    array[1] = x - y;
}

complex<float> floatfftstage::twiddler(int k)
{
    if (!out3.is_open()){
                  cout << "error reading " << outfile << endl;
                  assert(1 == 0);// "Could not open data/fft/input/input3.txt");
          }//If cannot read from file, return 1;
    float fW_cos;
    float fW_sin;
    float ftheta;
    ftheta = (2 * pi / N) * k;
    fW_cos = cos(ftheta);
    fW_sin = -sin(ftheta);
    int theta;
 //   complex<float> W(fW_cos, fW_sin);
    //return W;
    theta = (360  * k) >> int(log2(N));
    out3 << "N: " << N << " K: " << k << endl;
    out3 << "Theta: " << ftheta << " " << theta << endl;
    out3 << "Floating: " << fW_cos * 32768 << " " << fW_sin * 32768 << endl;

     int a[] = {0,572,1144,1715,2286,2856,3425,3993,4560,5126,5690,6252,6813,7371,7927,8481,9032,9580,10126,10668,11207,11743,12275,12803,13328,13848,14365,14876,15384,15886,16384,16877,17364,17847,18324,18795,19261,19720,20174,20622,21063,21498,21926,22348,22763,23170,23571,23965,24351,24730,25102,25466,25822,26170,26510,26842,27166,27482,27789,28088,28378,28660,28932,29197,29452,29698,29935,30163,30382,30592,30792,30983,31164,31336,31499,31651,31795,31928,32052,32166,32270,32365,32449,32524,32588,32643,32688,32723,32748,32763,32768};

        //  int a = pow(2,15);
        //  int scale = a/1000.0;

            int W_cos;
            int W_sin;

          //  theta = (360 / N) * k;
            float th;
            //theta = (360 >> int(log2(N))) * k;
              // theta = 20 * th;

            if (theta > 90) {
                W_cos = -a[-90 + theta];
                W_sin = -a[180 - theta];
            } else {
                W_cos = a[90 - theta];
                W_sin = -a[theta];
            }
          //  cout << "Inputs:" << N << " Theta: " << theta << endl;
            out3 << "Lookup: " << W_cos << " " << W_sin << endl << endl;
            complex<float>  W(W_cos, W_sin);
            return W;

}

void floatfftstage::output(complex<float> x)
{
    while (!next->ready) {
        //cout << N << "X" << endl;
        next->inputandtick(0);
    }
    next->inputandtick(x);
}

void floatfftstage::inputandtick(complex<float> x)
{

    //	cout << "FFT(" << N << ") starting in state " << state << " with x " << x << endl;

    if (state != FFT_STATE_OUTPUT && x == complex<float>(0, 0)) {
        cout << "PROBLEMS " << endl;
    }

    complex<float> butterflyresult[2];
    complex<float> outputtemp;

    int i;

    switch (state) {
        default:
        case FFT_STATE_INITIAL:
            //cout << N << "a" << endl;

            memory[write_pointer] = x;

            if (write_pointer == ((N / 2) - 1)) {
                state = FFT_STATE_READ;
                write_pointer = 0;
            } else {
                write_pointer++;
            }

            break;
        case FFT_STATE_READ:
           // cout << N << "b" << endl;

            butterfly(butterflyresult, memory[read_pointer], x);
            memory[write_pointer] = butterflyresult[1];

            //		cout << "butterfly output " << butterflyresult[0] << endl;
            output(butterflyresult[0]);

            if (read_pointer == ((N / 2) - 1)) {
                state = FFT_STATE_OUTPUT;
                write_pointer = 0;
                read_pointer = 0;
            } else {
                read_pointer++;
                write_pointer++;
            }

            break;
        case FFT_STATE_OUTPUT:
            //cout << N << "c" << endl;
            outputtemp = memory[read_pointer] * twiddler(read_pointer);
//            if (memory[read_pointer].real() > 1 << 16) {
//                outputtemp = memory[read_pointer]/complex<float> (32768,0) * twiddler(read_pointer);
//            }
//
//            else {
//                outputtemp = memory[read_pointer] * twiddler(read_pointer);
//                outputtemp =  outputtemp/complex<float> (32768,0);
//            }
//

            outputtemp = outputtemp/complex<float> (32768,0);
            if (memory[read_pointer].real() > 1<<29)
            {

            }
            output(outputtemp);

            if (read_pointer == ((N / 2) - 1)) {

                state = FFT_STATE_INITIAL;
                write_pointer = 0;
                read_pointer = 0;
            } else {
                read_pointer++;
            }

            break;
    }

    clock++;
    ready = (state != FFT_STATE_OUTPUT);

}

floatfftprint::floatfftprint(int Ninput)
{
    N = Ninput;
    count = 0;
    ready = 1;
}

void floatfftprint::inputandtick(complex<float> x)
{
	//cout << "output[" << count << "]: " << x << endl;
	answers.push(x);
    count++;
}

floatfftbuffer::floatfftbuffer(int Ninput)
{
    N = Ninput;
    count = 0;
    ready = 1;
}

void floatfftbuffer::inputandtick(complex<float> x)
{
    buf.push_back(x);
    count++;
}

floatfft::floatfft(int Ninput) :
        printer(Ninput)
{
    N = Ninput;
    stagecount = log2(N);
    int stagesize = 0;

    stages = new floatfftstage[stagecount];

    int i;
    for (i = 0; i < stagecount; i++) {
        stagesize = pow(2, (stagecount - i));
        stages[i].init(stagesize);

        if (i > 0) {
            stages[i - 1].next = &stages[i];
        }
    }

    stages[stagecount - 1].next = &printer;
}

void floatfft::inputandtick(complex<float> x)
{
    while (!stages[0].ready) {
        stages[0].inputandtick(0);
    }
    stages[0].inputandtick(x);
}

