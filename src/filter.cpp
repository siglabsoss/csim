#include "fixedfir.h"
using namespace std;




#define FILTER_LEN  63
int16_t coeffs[ FILTER_LEN ] =
{
 -1468, 1058,   594,   287,    186,  284,   485,   613,
   495,   90,  -435,  -762,   -615,   21,   821,  1269,
   982,    9, -1132, -1721,  -1296,    1,  1445,  2136,
  1570,    0, -1666, -2413,  -1735,   -2,  1770,  2512,
  1770,   -2, -1735, -2413,  -1666,    0,  1570,  2136,
  1445,    1, -1296, -1721,  -1132,    9,   982,  1269,
   821,   21,  -615,  -762,   -435,   90,   495,   613,
   485,  284,   186,   287,    594, 1058, -1468
};


#define SAMPLES   80

int main()
{
	
    int size;
    int16_t input[SAMPLES];
    int16_t output[SAMPLES];
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
    firFixedInit();
    
    

    // process all of the samples
    do {
        // read samples from file
        size = fread( input, sizeof(int16_t), SAMPLES, in_fid );
        // perform the filtering
        firFixed( coeffs, input, output, size, FILTER_LEN );
        // write samples to file
        fwrite( output, sizeof(int16_t), size, out_fid );
    } while ( size != 0 );

    fclose( in_fid );
    fclose( out_fid );

    
    return 0;
}





