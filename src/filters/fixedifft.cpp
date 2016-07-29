#include <iostream>
#include <cmath>
using namespace std;
#include <filters/fixedifft.hpp>

const float pi = 3.14159265359;


bool fixedifftbase::input(const filter_io_t &data)
{

}

bool fixedifftbase::output(filter_io_t &data)
{


}


void fixedifftbase::tick()
{

}

bool fixedifft::input(const filter_io_t &data)
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

bool fixedifft::output(filter_io_t &data)
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

void fixedifft::tick()
{

}

fixedifftbase::~fixedifftbase()
{

}

void fixedifftstage::init(int Ninput)
{
    N = Ninput;

    memory = new FixedComplex<32> [N / 2];

    state = Fifft_STATE_INITIAL;

    read_pointer = 0;
    write_pointer = 0;

    ready = 1;

    clock = 0;

//	cout << "ifft bufferfly " << N << " was created" << endl;
}

fixedifftstage::fixedifftstage(int Ninput)
{
    init(Ninput);
}

fixedifftstage::fixedifftstage()
{

}

void fixedifftstage::dump(void)
{
    int i;
    for (i = 0; i < N / 2; i++) {
        cout << "Memory " << i << ": " << memory[i] << endl;
    }
}

void fixedifftstage::butterfly(FixedComplex<32> array[2], FixedComplex<32> x,
        FixedComplex<32> y)
{
    array[0] = x + y;
    array[1] = x - y;
}

FixedComplex<32> fixedifftstage::twiddler(int k)
{

        //int scale = 32;
    int a[] = {0,4,9,13,18,22,27,31,36,40,44,49,53,58,62,66,71,75,79,83,88,92,96,100,104,108,112,116,120,124,128,132,136,139,143,147,150,154,158,161,165,168,171,175,178,181,184,187,190,193,196,199,202,204,207,210,212,215,217,219,222,224,226,228,230,232,234,236,237,239,241,242,243,245,246,247,248,249,250,251,252,253,254,254,255,255,255,256,256,256,256
};
    //  int a = pow(2,15);
    //  int scale = a/1000.0;

        int W_cos;
        int W_sin;
        int theta;
      //  theta = (360 / N) * k;
        theta = (360 >> int(log2(N))) * k;


        if (theta > 90) {
            W_cos = -a[-90 + theta];
            W_sin = a[180 - theta];
        } else {
            W_cos = a[90 - theta];
            W_sin = a[theta];
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

void fixedifftstage::output(FixedComplex<32> x)
{
    while (!next->ready) {
       // cout << N << "X" << endl;
        next->inputandtick(FixedComplex<32>(0, 0));
    }
    next->inputandtick(x);
}

void fixedifftstage::inputandtick(FixedComplex<32> x)
{

    //	cout << "ifft(" << N << ") starting in state " << state << " with x " << x << endl;

    if (state != Fifft_STATE_OUTPUT && x == FixedComplex<32>(0, 0)) {
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
        case Fifft_STATE_INITIAL:
           // cout << N << "a" << endl;

            memory[write_pointer] = x;

            if (write_pointer == ((N / 2) - 1)) {
                state = Fifft_STATE_READ;
                write_pointer = 0;
            } else {
                write_pointer++;
            }

            break;
        case Fifft_STATE_READ:
            //cout << N << "b" << endl;

            butterfly(butterflyresult, memory[read_pointer], x);
            memory[write_pointer] = butterflyresult[1];

            //		cout << "butterfly output " << butterflyresult[0] << endl;
            output(butterflyresult[0]);

            if (read_pointer == ((N / 2) - 1)) {
                state = Fifft_STATE_OUTPUT;
                write_pointer = 0;
                read_pointer = 0;
            } else {
                read_pointer++;
                write_pointer++;
            }

            break;
        case Fifft_STATE_OUTPUT:
            //cout << N << "c" << endl;
            if (N == 8) {
                int dontcare = 10;
            }
//		as = memory[read_pointer];
//		as = as/scale;
            outputtemp = memory[read_pointer] * twiddler(read_pointer);

            outputtemp = (outputtemp >> 8);
            output(outputtemp);
         //   cout << "N: " << N << " outputtemp: " << outputtemp;
            if (read_pointer == ((N / 2) - 1)) {

                state = Fifft_STATE_INITIAL;
                write_pointer = 0;
                read_pointer = 0;
            } else {
                read_pointer++;
            }

            break;
    }

    clock++;
    ready = (state != Fifft_STATE_OUTPUT);

}

fixedifftprint::fixedifftprint(int Ninput)
{
    N = Ninput;
    count = 0;
    ready = 1;
}

void fixedifftprint::inputandtick(FixedComplex<32> x)
{
//	sc_int<32> a = pow(2,15);
//	x = x * a;
//   cout << "output[" << count << "]: " << x.real.to_int()<< ","
//            << x.imag.to_int() << " " << x << endl;

    m_output.push(x.to_32());//adds this output to the queue of outputs
    count++;
}

fixedifftbuffer::fixedifftbuffer(int Ninput)
{
    N = Ninput;
    count = 0;
    ready = 1;
}

void fixedifftbuffer::inputandtick(FixedComplex<32> x)
{
    buf.push_back(x);
    count++;
}

fixedifft::fixedifft(int Ninput) :
        printer(Ninput)
{

    m_count = 0;
    N = Ninput;
    stagecount = log2(N);
    int stagesize = 0;

    stages = new fixedifftstage[stagecount];

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

void fixedifft::inputandtick(FixedComplex<32> x)
{
//	cout << "input was " << x.real << endl;
//	sc_int<32> a = pow(2,15);
//	x = x / a;
    while (!stages[0].ready) {
        stages[0].inputandtick(FixedComplex<32>(0, 0));
    }
    stages[0].inputandtick(x);
}

