#include "fixedfir.h"
using namespace std;







#define SAMPLES   80

int main()
{
	
    int size;
    FixedComplex<16> input[SAMPLES];
    FixedComplex<16> output[SAMPLES];
    FILE   *in_fid;
    FILE   *out_fid;

    // open the input waveform file
    in_fid = fopen( "input.pcm", "rb" );
    if ( in_fid == 0 ) {
        printf("couldn't open input.pcm");
        exit(EXIT_FAILURE);
    }

    // open the output waveform file
    out_fid = fopen( "outputFixed.pcm", "wb" );
    if ( out_fid == 0 ) {
        printf("couldn't open outputFixed.pcm");
        exit(EXIT_FAILURE);
    }

    // initialize the filter

    
    

    // process all of the samples
    do {
        // read samples from file
        size = fread( input, sizeof(FixedComplex<16>), SAMPLES, in_fid );
        fixedfir fir(size, input);
        fir.firFixedInit();
        // perform the filtering
        fir.fir(output);
        // write samples to file
        fwrite( output, sizeof(FixedComplex<16>), size, out_fid );
    } while ( size != 0 );

    fclose( in_fid );
    fclose( out_fid );

    
    return 0;
}





