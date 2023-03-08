#include <stdio.h>
#include <fftw3.h>

#define FILENAME "noisy.cf32"
#define SAMPLE_FREQ 4000000
#define LENGTH 32

int main(void)
{
	fftwf_complex *in, *out;
	fftwf_plan p;
	in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * LENGTH);
    out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * LENGTH);
    p = fftwf_plan_dft_1d(LENGTH, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
	for(int i=0; i<LENGTH; i++)
	{
		if(i%2==0)
		{
			in[i][0] = 1.0f;
			in[i][1] = 0.0f;
		}
		else
		{
			in[i][0] = -1.0f;
			in[i][1] = 0.0f;
		}
	}

    fftwf_execute(p); /* repeat as needed */
    
    fftwf_destroy_plan(p);
	
	for(int i=0; i<LENGTH; i++)
	{
		printf("%04.4f + %04.4fi\n",out[i][0],out[i][1]);
	}

    fftwf_free(in); fftwf_free(out);
	return 0;
}
