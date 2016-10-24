#include <filters/bfp_fft.hpp>


int main(int argc, char *argv[])
{
    BFPFFT fft(1024, false);

    fft.printTwiddleFactors();
    return 0;
}
