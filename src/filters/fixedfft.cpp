#include <iostream>
#include <cmath>
using namespace std;

#include <filters/fixedfft.hpp>

const float pi = 3.14159265359;


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

        //int scale = 32;
        int a[] = {0,1,1,2,2,3,3,4,4,5,6,6,7,7,8,8,9,9,10,10,11,11,12,13,13,14,14,15,15,16,16,16,17,17,18,18,19,19,20,20,21,21,21,22,22,23,23,23,24,24,25,25,25,26,26,26,27,27,27,27,28,28,28,29,29,29,29,29,30,30,30,30,30,31,31,31,31,31,31,31,32,32,32,32,32,32,32,32,32,32,32};
    //  int a = pow(2,15);
    //  int scale = a/1000.0;

        int W_cos;
        int W_sin;
        int theta;
      //  theta = (360 / N) * k;
        theta = (360 >> int(log2(N))) * k;


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

    }

//
//    int thet;
//    thet = (360 / N) * k * 572; //2pi * 32768 / 360 = 572
//    cordic c;
//    sc_int<32> sin;
//    sc_int<32> cos;
//    sin = c.sin(thet);//Calculates sin
//    int tempsin = sin * -1024 / 32768;//Scales to 1024
//    int phaseShift = 51471;//pi/2 * 32768
//    cos = c.sin(thet + phaseShift);//Calculates cos
//    int tempcos = cos * 1024 / 32768;//Scales to 1024
//
//    FixedComplex<32> V(tempcos,tempsin);
//    return V;
//}

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

    if (state != FFFT_STATE_OUTPUT && x == FixedComplex<32>(0, 0)) {
        cout << "PROBLEMS " << endl;
    }

    FixedComplex<32> butterflyresult[2];
    FixedComplex<32> outputtemp;

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
            outputtemp = memory[read_pointer] * twiddler(read_pointer);

            outputtemp = outputtemp >> 5;
            output(outputtemp);
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
    cout << "output[" << count << "]: " << x.real.to_int()<< ","
            << x.imag.to_int() << " " << x << endl;

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

