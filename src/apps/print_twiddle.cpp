#include <filters/fft.hpp>


int main(int argc, char *argv[])
{
    FFT fft(1024, false);

    fft.printTwiddleFactors();
    return 0;
}
