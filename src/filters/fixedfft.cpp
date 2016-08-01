#include <iostream>
#include <cmath>
using namespace std;
#include <filters/fixedfft.hpp>

const float pi = 3.14159265359;
//string outfile("../csim/data/fft/output/compareTwiddler.txt");
//ofstream out3(outfile.c_str());

bool fixedfftbase::input(const filter_io_t &data)
{

}

bool fixedfftbase::output(filter_io_t &data)
{


}


void fixedfftbase::tick()
{

}

bool fixedfft::input(const filter_io_t &data)
{
    m_count++;//One more input has been received
    assert(data.type == IO_TYPE_FIXED_COMPLEX_32);
    FixedComplex<32> sample = data.fc32;
    inputandtick(sample);
    return true;
}

/**
 * output - provide an output sample to the caller.
 * @return false if no output sample is available.
 */

bool fixedfft::output(filter_io_t &data)
{
    if (m_count > (2*N) - 1) {
        m_count = m_count - (2*N); //Full cycle of outputs complete
    }

    if (m_count >= N) {
        data.type = IO_TYPE_FIXED_COMPLEX_32;

        data.fc32 = printer.m_output.front();
        printer.m_output.pop();

        return true;
    }//Time to start outputs
    else {
        return false;
    }//Not ready
}

void fixedfft::tick()
{

}

fixedfftbase::~fixedfftbase()
{

}

void fixedfftstage::init(int Ninput)
{
    N = Ninput;

    memory = new FixedComplex<32> [N / 2];

    state = FFFT_STATE_INITIAL;

    read_pointer = 0;
    write_pointer = 0;

    ready = 1;

    clock = 0;

//	cout << "FFT bufferfly " << N << " was created" << endl;
}

fixedfftstage::fixedfftstage(int Ninput)
{
    init(Ninput);
}

fixedfftstage::fixedfftstage()
{

}

void fixedfftstage::dump(void)
{
    int i;
    for (i = 0; i < N / 2; i++) {
        cout << "Memory " << i << ": " << memory[i] << endl;
    }
}

void fixedfftstage::butterfly(FixedComplex<32> array[2], FixedComplex<32> x,
        FixedComplex<32> y)
{
    array[0] = x + y;
    array[1] = x - y;
}

FixedComplex<32> fixedfftstage::twiddler(int k)
{
        int a[] = {0,572,1144,1715,2286,2856,3425,3993,4560,5126,5690,6252,6813,7371,7927,8481,9032,9580,10126,10668,11207,11743,12275,12803,13328,13848,14365,14876,15384,15886,16384,16877,17364,17847,18324,18795,19261,19720,20174,20622,21063,21498,21926,22348,22763,23170,23571,23965,24351,24730,25102,25466,25822,26170,26510,26842,27166,27482,27789,28088,28378,28660,28932,29197,29452,29698,29935,30163,30382,30592,30792,30983,31164,31336,31499,31651,31795,31928,32052,32166,32270,32365,32449,32524,32588,32643,32688,32723,32748,32763,32768};


        int W_cos;
        int W_sin;
        int theta;

        theta = (360  * k) >> int(log2(N));

        if (theta > 90) {
            W_cos = -a[-90 + theta];
            W_sin = -a[180 - theta];
        } else {
            W_cos = a[90 - theta];
            W_sin = -a[theta];
        }
      //  cout << "Inputs:" << N << " Theta: " << theta << endl;
        FixedComplex<32> W(W_cos, W_sin);
        return W;
// //For using cordic

//    int thet;
//    thet = (2 * k * 32768 * pi / N);
//    cordic c;
//    sc_int<32> sin1;
//    sc_int<32> cos1;
//    sin1 = -c.sin(thet);//Calculates sin
//    int phaseShift = 51471;//pi/2 * 32768
//    cos1 = c.sin(thet + phaseShift);//Calculates cos
//    FixedComplex<32> V(cos1,sin1);


// // For printing out true sin and cos values
//    float fW_cos;
//    float fW_sin;
//    float ftheta;
//    ftheta = (2 * pi / N) * k;
//    fW_cos = cos(ftheta);
//    fW_sin = -sin(ftheta);
//    out3 << "Theta: " << theta << "  " << ftheta << endl << W << V << "Cos: " << fW_cos * 32768 << " Sin: " << fW_sin  * 32768<< endl << endl;
//    return V;
}

void fixedfftstage::output(FixedComplex<32> x)
{
    while (!next->ready) {
       // cout << N << "X" << endl;
        next->inputandtick(FixedComplex<32>(0, 0));
    }
    next->inputandtick(x);
}

void fixedfftstage::inputandtick(FixedComplex<32> x)
{

    //	cout << "FFT(" << N << ") starting in state " << state << " with x " << x << endl;

//    if (state != FFFT_STATE_OUTPUT && x == FixedComplex<32>(0, 0)) {
//        cout << "PROBLEMS " << endl;
//    }

    FixedComplex<32> butterflyresult[2];
    FixedComplex<64> outputtemp;

    int i;
    int as;
    int a = pow(2, 15);
    int scale = a / 1000.0;
    scale = 32;//Cannot scale higher that this yet
    switch (state) {
        default:
        case FFFT_STATE_INITIAL:
           // cout << N << "a" << endl;

            memory[write_pointer] = x;

            if (write_pointer == ((N / 2) - 1)) {
                state = FFFT_STATE_READ;
                write_pointer = 0;
            } else {
                write_pointer++;
            }

            break;
        case FFFT_STATE_READ:
            //cout << N << "b" << endl;

            butterfly(butterflyresult, memory[read_pointer], x);
            memory[write_pointer] = butterflyresult[1];

            //		cout << "butterfly output " << butterflyresult[0] << endl;
            output(butterflyresult[0]);

            if (read_pointer == ((N / 2) - 1)) {
                state = FFFT_STATE_OUTPUT;
                write_pointer = 0;
                read_pointer = 0;
            } else {
                read_pointer++;
                write_pointer++;
            }

            break;
        case FFFT_STATE_OUTPUT:
            //cout << N << "c" << endl;
            if (N == 8) {
                int dontcare = 10;
            }
//		as = memory[read_pointer];
//		as = as/scale;
            outputtemp = (memory[read_pointer].to_64() * twiddler(read_pointer).to_64());
            if (memory[read_pointer].real > (1<<22) || memory[read_pointer].imag > (1<<22) ) {
                cout << "PROBLEM2" << endl;
                cout << memory[read_pointer];
            }
//            outputtemp = outputtemp >> 5;
            outputtemp = outputtemp >> 15;
            output(outputtemp.to_32());
         //   cout << "N: " << N << " outputtemp: " << outputtemp;
            if (read_pointer == ((N / 2) - 1)) {

                state = FFFT_STATE_INITIAL;
                write_pointer = 0;
                read_pointer = 0;
            } else {
                read_pointer++;
            }

            break;
    }

    clock++;
    ready = (state != FFFT_STATE_OUTPUT);

}

fixedfftprint::fixedfftprint(int Ninput)
{
    N = Ninput;
    count = 0;
    ready = 1;
}

void fixedfftprint::inputandtick(FixedComplex<32> x)
{
//	sc_int<32> a = pow(2,15);
//	x = x * a;
//   cout << "output[" << count << "]: " << x.real.to_int()<< ","
//            << x.imag.to_int() << " " << x << endl;

    m_output.push(x.to_32());//adds this output to the queue of outputs
    count++;
}

fixedfftbuffer::fixedfftbuffer(int Ninput)
{
    N = Ninput;
    count = 0;
    ready = 1;
}

void fixedfftbuffer::inputandtick(FixedComplex<32> x)
{
    buf.push_back(x);
    count++;
}

fixedfft::fixedfft(int Ninput) :
        printer(Ninput)
{

    m_count = 0;
    N = Ninput;
    stagecount = log2(N);
    int stagesize = 0;

    stages = new fixedfftstage[stagecount];

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

void fixedfft::inputandtick(FixedComplex<32> x)
{
//	cout << "input was " << x.real << endl;
//	sc_int<32> a = pow(2,15);
//	x = x / a;
    while (!stages[0].ready) {
        stages[0].inputandtick(FixedComplex<32>(0, 0));
    }
    stages[0].inputandtick(x);
}

