#include <iostream>
#include <cmath>
using namespace std;
#include <filters/fixedfft.hpp>

string outfile("../csim/data/fft/output/compareTwiddler.txt");
ofstream out3(outfile.c_str());

bool fixedfft::input(const filter_io_t &data)
{
    m_count++;//One more input has been received
    newInput = true;
    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW);
    FixedComplex32 sample = data.fcn32;

    while (!stages[0].ready) {
        stages[0].inputandtick(FixedComplex32(0, 0));
    }
    stages[0].inputandtick(sample);
    return true;
}

/**
 * output - provide an output sample to the caller.
 * @return false if no output sample is available.
 */

bool fixedfft::output(filter_io_t &data)
{
    static unsigned int count = 0;

    if (newInput == true) {
        newInput = false;
        if (m_count > N) {
            data = printer.m_output.front();
            printer.m_output.pop();
            //std::cout << count % 8 << ": "<< data << std::endl;
            count++;
            return true;
        }//Time to start outputs
    }

    return false;
}

fixedfft::fixedfft(int Ninput, int tableSize, bool inverse) :
        FilterChainElement("FixedFFT"),
        N(Ninput),
        stagecount(log2(N)),
        newInput(false),
        m_count(0),
        mainTable(),
        stages(Ninput),
        printer(Ninput, inverse),
        m_inverse(inverse)
{


    if (tableSize == 0) {
        if (Ninput < 32) {
            tableSize = 180 * 32;
        }
        else {
            tableSize = 180 * Ninput >> 4;
        }
    }
    mainTable = std::vector<int>(tableSize);
    for (int i = 0; i < tableSize + 1; i++) {
        mainTable[i] = (round(sin(((M_PI/tableSize) * i)/2) * 32768)); //Get values of main table.
    }

    int stagesize = 0;
    for (int i = 0; i < stagecount; i++) {
        stagesize = pow(2, (stagecount - i));
        stages[i].init(stagesize, inverse);
        stages[i].tableSize = tableSize;
        stages[i].mainTablePointer = &mainTable; //XXX do this differently
        if (i > 0) {
            stages[i - 1].next = &stages[i];
        }
    }
    stages[stagecount - 1].next = &printer;
}



void fixedfft::tick()
{

}

//////////////////////////////////////////////////////////////////////////

void fixedfftstage::init(int Ninput, bool inverse)
{
	m_inverse = inverse;
    N = Ninput;
    memory = std::vector<FixedComplex32>(N / 2);
    state = FFFT_STATE_INITIAL;
    read_pointer = 0;
    write_pointer = 0;
    ready = 1;
    clock = 0;
//	cout << "FFT bufferfly " << N << " was created" << endl;
}

fixedfftstage::fixedfftstage() :
        N(0),
        memory(),
        state(FFFT_STATE_INITIAL),
        read_pointer(0),
        write_pointer(0),
        clock(0),
        next(nullptr),
        theta(0),
        tableSize(0),
        mainTablePointer(nullptr)
{
}

void fixedfftstage::dump(void)
{
    for (int i = 0; i < N / 2; i++) {
        cout << "Memory " << i << ": " << memory[i] << endl;
    }
}

void fixedfftstage::butterfly(FixedComplex32 array[2], FixedComplex32 x,
        FixedComplex32 y)
{
    array[0] = x + y;
    array[1] = x - y;
}

FixedComplex32 fixedfftstage::twiddler(int k)
{
    FixedComplex32 W;
    int increment1 = (360 * ((int)tableSize/90)) / (N);

    int increment = (360*256) / (N);

    int * b = new int[(N/4) + 1];
    for (int i = 0; i <= ((N/4)); i++) {
        b[i] = (*mainTablePointer)[i * increment1];
    }

    if (k > (N/4)) {
        W.real(-b[(-N/4) + k ] / 32768.0);
        W.imag(-b[(N/2) - k] / 32768.0);
    }
    else {
		if (N == 2) {
			W.real(1.0);
			W.imag(0.0);
		}
		else {
			W.real(b[(N/4) - k] / 32768.0);
			W.imag(-b[k] / 32768.0);
		}
      }

    if (m_inverse)
    {
    	W.imag( W.imag() * -1);
    }

    delete[] b;
    return W; // return lookup table value
}

void fixedfftstage::output(FixedComplex32 x)
{
    while (!next->ready) {
       // cout << N << "X" << endl;
        next->inputandtick(FixedComplex32(0, 0));
    }
    next->inputandtick(x);
}

void fixedfftstage::inputandtick(FixedComplex32 x)
{

    //	cout << "FFT(" << N << ") starting in state " << state << " with x " << x << endl;

//    if (state != FFFT_STATE_OUTPUT && x == FixedComplex32(0, 0)) {
//        cout << "PROBLEMS " << endl;
//    }

    FixedComplex32 butterflyresult[2];
    FixedComplex32 outputtemp;

    int i;
    int as;
    //int a = pow(2, 15);
    //int scale = a / 1000.0;
    //scale = 32;//Cannot scale higher that this yet
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
            //if (N == 8) {
            //    int dontcare = 10;
            //}
            output((memory[read_pointer] * twiddler(read_pointer)));
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

//////////////////////////////////////////////////////////////////////////

fixedfftprint::fixedfftprint(int Ninput, bool inverse) :
		m_inverse(inverse)
{

    N = Ninput;
    count = 0;
    ready = 1;
}

void fixedfftprint::inputandtick(FixedComplex32 x)
{
	if (m_inverse == false) {
		m_output.push(x);//adds this output to the queue of outputs
	}//FFT
	else {
		m_output.push(FixedComplex32(x.real()/N, x.imag()/N));//adds this output to the queue of outputs
	}//IFFT
    count++;
}
