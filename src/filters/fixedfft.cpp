#include <iostream>
#include <cmath>
#include <filters/fixedfft.hpp>

bool fixedfft::input(const filter_io_t &data)
{
    m_count++;//One more input has been received
    newInput = true; //Input has been received so an output may be taken
    assert(data.type == IO_TYPE_FIXED_COMPLEX_32_NEW);
    FixedComplex32 sample = data.fcn32;
    while (!stages[0].ready) {
        stages[0].inputandtick(FixedComplex32(0, 0));
    }
    stages[0].inputandtick(sample);//Perform fft
    return true;
}

bool fixedfft::output(filter_io_t &data)
{
    if (newInput == true) {
        newInput = false; //Next output may not be taken until new input is given
        if (m_count > N) {
            data = printer.m_output.front(); //Get output
            printer.m_output.pop(); //Remove from queue
            return true; //Output was sent
        }//If number of inputs is greater than the number of points in the fft
    }
    return false; //Outputs not ready
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
        }// Cannot right shift by 4
        else {
            tableSize = 180 * Ninput >> 4;
        }//table size reaches upper limit of precision
    }//If no table size specified

    mainTable = std::vector<int>(tableSize);
    for (int i = 0; i < tableSize + 1; i++) {
        mainTable[i] = (round(sin(((M_PI/tableSize) * i)/2) * 32768)); //Get values of main table. Scaled by 32768 for high precision
    }

    int stagesize = 0;
    for (int i = 0; i < stagecount; i++) {
        stagesize = pow(2, (stagecount - i));
        stages[i].init(stagesize, inverse);
        if (i > 0) {
            stages[i - 1].next = &stages[i];
        }
        stages[i].mainTablePointer = &mainTable; //Sets pointer to twiddle factor lookup table
    }
    stages[stagecount - 1].next = &printer;
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
}

fixedfftstage::fixedfftstage() :
        N(0),
        memory(),
        state(FFFT_STATE_INITIAL),
        read_pointer(0),
        write_pointer(0),
        clock(0),
        next(nullptr),
        mainTablePointer(nullptr),
        m_inverse(false)
{}

void fixedfftstage::butterfly(FixedComplex32 array[2], FixedComplex32 x,
        FixedComplex32 y)
{
    array[0] = x + y;
    array[1] = x - y;
}

FixedComplex32 fixedfftstage::twiddler(int k)
{
    FixedComplex32 W;
    int increment = (360 * ((int)(*mainTablePointer).size()/90)) / (N);
    int* b = new int[(N/4) + 1];
    for (int i = 0; i <= ((N/4)); i++) {
        b[i] = (*mainTablePointer)[i * increment];
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

    if (m_inverse) {
    	W.imag( W.imag() * -1);
    }//Convert to IFFT

    delete[] b;
    return W; // return lookup table value
}//Performs bulk of calculations

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
    FixedComplex32 butterflyresult[2];
    FixedComplex32 outputtemp;
    int i;
    int as;
    switch (state) {
        case FFFT_STATE_INITIAL:

            memory[write_pointer] = x;
            if (write_pointer == ((N / 2) - 1)) {
                state = FFFT_STATE_READ;
                write_pointer = 0;
            } else {
                write_pointer++;
            }
            break;

        case FFFT_STATE_READ:
            butterfly(butterflyresult, memory[read_pointer], x);
            memory[write_pointer] = butterflyresult[1];
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
            output((memory[read_pointer] * twiddler(read_pointer)));
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

void fixedfft::tick()
{}

//////////////////////////////////////////////////////////////////////////

fixedfftprint::fixedfftprint(int Ninput, bool inverse) :
		m_inverse(inverse)
{
    N = Ninput;
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
}
