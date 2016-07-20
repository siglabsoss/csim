#include <iostream>
#include <cmath>
#include "fixedcomplex.h"
using namespace std;

#include "fixedfft.h"
const float pi = 3.14159265359;


ofstream out2("out1.csv");

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
    int a[] = { 0, 0.55848, 1.1168, 1.6748, 2.2322, 2.789, 3.3449, 3.8998,
            4.4535, 5.0059, 5.5567, 6.1059, 6.6532, 7.1984, 7.7415, 8.2822,
            8.8204, 9.3559, 9.8885, 10.418, 10.945, 11.468, 11.987, 12.503,
            13.016, 13.524, 14.028, 14.528, 15.023, 15.514, 16, 16.481, 16.957,
            17.428, 17.894, 18.354, 18.809, 19.258, 19.701, 20.138, 20.569,
            20.994, 21.412, 21.824, 22.229, 22.627, 23.019, 23.403, 23.781,
            24.151, 24.513, 24.869, 25.216, 25.556, 25.889, 26.213, 26.529,
            26.837, 27.138, 27.429, 27.713, 27.988, 28.254, 28.512, 28.761,
            29.002, 29.233, 29.456, 29.67, 29.875, 30.07, 30.257, 30.434,
            30.602, 30.76, 30.91, 31.049, 31.18, 31.301, 31.412, 31.514, 31.606,
            31.689, 31.761, 31.825, 31.878, 31.922, 31.956, 31.981, 31.995, 32,
            31.995, 31.981, 31.956, 31.922, 31.878, 31.825, 31.761, 31.689,
            31.606, 31.514, 31.412, 31.301, 31.18, 31.049, 30.91, 30.76, 30.602,
            30.434, 30.257, 30.07, 29.875, 29.67, 29.456, 29.233, 29.002,
            28.761, 28.512, 28.254, 27.988, 27.713, 27.429, 27.138, 26.837,
            26.529, 26.213, 25.889, 25.556, 25.216, 24.869, 24.513, 24.151,
            23.781, 23.403, 23.019, 22.627, 22.229, 21.824, 21.412, 20.994,
            20.569, 20.138, 19.701, 19.258, 18.809, 18.354, 17.894, 17.428,
            16.957, 16.481, 16, 15.514, 15.023, 14.528, 14.028, 13.524, 13.016,
            12.503, 11.987, 11.468, 10.945, 10.418, 9.8885, 9.3559, 8.8204,
            8.2822, 7.7415, 7.1984, 6.6532, 6.1059, 5.5567, 5.0059, 4.4535,
            3.8998, 3.3449, 2.789, 2.2322, 1.6748, 1.1168, 0.55848, 3.9189e-15,
            -0.55848, -1.1168, -1.6748, -2.2322, -2.789, -3.3449, -3.8998,
            -4.4535, -5.0059, -5.5567, -6.1059, -6.6532, -7.1984, -7.7415,
            -8.2822, -8.8204, -9.3559, -9.8885, -10.418, -10.945, -11.468,
            -11.987, -12.503, -13.016, -13.524, -14.028, -14.528, -15.023,
            -15.514, -16, -16.481, -16.957, -17.428, -17.894, -18.354, -18.809,
            -19.258, -19.701, -20.138, -20.569, -20.994, -21.412, -21.824,
            -22.229, -22.627, -23.019, -23.403, -23.781, -24.151, -24.513,
            -24.869, -25.216, -25.556, -25.889, -26.213, -26.529, -26.837,
            -27.138, -27.429, -27.713, -27.988, -28.254, -28.512, -28.761,
            -29.002, -29.233, -29.456, -29.67, -29.875, -30.07, -30.257,
            -30.434, -30.602, -30.76, -30.91, -31.049, -31.18, -31.301, -31.412,
            -31.514, -31.606, -31.689, -31.761, -31.825, -31.878, -31.922,
            -31.956, -31.981, -31.995, -32, -31.995, -31.981, -31.956, -31.922,
            -31.878, -31.825, -31.761, -31.689, -31.606, -31.514, -31.412,
            -31.301, -31.18, -31.049, -30.91, -30.76, -30.602, -30.434, -30.257,
            -30.07, -29.875, -29.67, -29.456, -29.233, -29.002, -28.761,
            -28.512, -28.254, -27.988, -27.713, -27.429, -27.138, -26.837,
            -26.529, -26.213, -25.889, -25.556, -25.216, -24.869, -24.513,
            -24.151, -23.781, -23.403, -23.019, -22.627, -22.229, -21.824,
            -21.412, -20.994, -20.569, -20.138, -19.701, -19.258, -18.809,
            -18.354, -17.894, -17.428, -16.957, -16.481, -16, -15.514, -15.023,
            -14.528, -14.028, -13.524, -13.016, -12.503, -11.987, -11.468,
            -10.945, -10.418, -9.8885, -9.3559, -8.8204, -8.2822, -7.7415,
            -7.1984, -6.6532, -6.1059, -5.5567, -5.0059, -4.4535, -3.8998,
            -3.3449, -2.789, -2.2322, -1.6748, -1.1168, -0.55848, -7.8377e-15 };
//	int a = pow(2,15);
//	int scale = a/1000.0;

    int W_cos;
    int W_sin;
    int theta;

    theta = (360 / N) * k;

//
//    cordic c;
//    FixedComplex<16> a(1, 0);
//    FixedComplex<16> b(0, 1);
//
//    sc_int<32> sin;
//    sc_int<32> cos;
// //   int newTheta = theta * 32768 /360 * 2 * 3.14159;
//    sin = c.sin(theta);
//    int tempsin = sin * 32 / 32768;
//    int phaseShift = 51471;
//    cos = c.sin(theta + phaseShift);
//    int tempcos = cos * 32 / 32768;
//
//
//
//          FixedComplex<32> W(tempcos,tempsin);
//          return W;



//	sc_int<32> as_cos, as_sin;
//	W_cos = cos(theta);
//	W_cos = W_cos * scale;
//	W_sin = -sin(theta);
//	W_sin = W_sin * scale;
    W_sin = -a[theta];
    if (theta > 90) {
        W_cos = -a[-90 + theta];
    } else {
        W_cos = a[90 - theta];
    }

    FixedComplex<32> W(W_cos, W_sin);
    return W;

}

void fixedfftstage::output(FixedComplex<32> x)
{
    while (!next->ready) {
        cout << N << "X" << endl;
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
    scale = 32;
    switch (state) {
        default:
        case FFFT_STATE_INITIAL:
            cout << N << "a" << endl;

            memory[write_pointer] = x;

            if (write_pointer == ((N / 2) - 1)) {
                state = FFFT_STATE_READ;
                write_pointer = 0;
            } else {
                write_pointer++;
            }

            break;
        case FFFT_STATE_READ:
            cout << N << "b" << endl;

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
            cout << N << "c" << endl;
            if (N == 8) {
                int dontcare = 10;
            }
//		as = memory[read_pointer];
//		as = as/scale;
            outputtemp = memory[read_pointer] * twiddler(read_pointer);

            outputtemp = outputtemp / FixedComplex<32>(scale, 0);
            output(outputtemp);

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
    cout << "output[" << count << "]: " << x.real.to_int() / 32.0 << ","
            << x.imag.to_int() / 32.0 << " " << x << endl;


    out2 << x.real.to_int() << "," << x.imag.to_int() << endl;
   // out.close();
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

