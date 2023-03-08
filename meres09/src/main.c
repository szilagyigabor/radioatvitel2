#include <stdio.h>
#include "kiss_fft.h"

#define FILENAME "noisy.cf32"
#define SAMPLE_FREQ 4000000
#define LENGTH 8

int main(void)
{
	kiss_fft_cfg cfg = kiss_fft_alloc(LENGTH, 0, NULL, NULL);
	kiss_fft_cpx complex_in[LENGTH], complex_out[LENGTH];
	kiss_fft_scalar one=1.0, zero=0.0;

	for(int i=0; i<LENGTH; i++)
	{
			complex_in[i].r=one;
			complex_in[i].i=zero;
	}

	kiss_fft(cfg, complex_in, complex_out);

	for(int i=0; i<LENGTH; i++)
	{
		printf("%2d  %04.4f + %04.4fi\n", i, complex_out[i].r, complex_out[i].i);
	}
	
    kiss_fft_free(cfg);
	return 0;
}
