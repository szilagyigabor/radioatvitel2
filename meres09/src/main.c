#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>

#define FILENAME "noisy.cf32"
#define SAMPLE_FREQ 4000000
#define LENGTH 32
#define PI 3.1415926535897932384626433832795f

int main(void)
{
	int kod_int[] = {3,2,2,2,1,2,3,0,3,3,0,3,2,0,1,3,0,2,3,1,0,1,2,3,2,1,1,1,1,2,0,3,2,2,3,3,3,1,3,0,2,1,2,3,2,2,2,2,2,0,3,1,2,0,0,3,1,3,2,3,0,3,2,2,1,1,2,0,2,1,1,0,1,0,3,2,2,2,1,0,1,3,2,2,0,3,2,2,3,1,1,2,2,1,3,2,0,3,1,1,3,0,3,1,2,1,3,2,2,3,2,3,2,1,2,0,0,3,1,1,0,1,2,1,1,1,1,1};
	
	// open file and determine number of complex samples
	FILE* noisy_file = fopen(FILENAME,"r");
	
	fseek(noisy_file, 0, SEEK_END);
	int N = ftell(noisy_file)/8;
	fseek(noisy_file, 0, SEEK_SET);
	
	fftwf_complex *samples = fftwf_malloc(sizeof(fftwf_complex) * N);	// read data from file
	size_t RET_CODE = fread(samples, sizeof(fftwf_complex), N, noisy_file);
	if(RET_CODE == N)
		printf("%s read successfully\n", FILENAME);
	else
		printf("error while reading %s\n", FILENAME);

	fclose(noisy_file);

	//fftwf_complex *in, *out;
	fftwf_plan pf, pi;
	//in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * LENGTH);
    //out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * LENGTH);
    pf = fftwf_plan_dft_1d(N, samples, samples, FFTW_FORWARD, FFTW_ESTIMATE);
    pi = fftwf_plan_dft_1d(N, samples, samples, FFTW_BACKWARD, FFTW_ESTIMATE);
    
    fftwf_execute(pf); 
    
    fftwf_destroy_plan(pf);
    fftwf_destroy_plan(pi);
	

	// print original spectrum
	if(1)
	{
		printf("writing to fft_utan.txt ...");
		FILE *fft_utan_fp = fopen("fft_utan.txt","w");
	
		for(int i=0; i<N; i+=512)
			fprintf(fft_utan_fp, "%d %f %f %f\n", i, samples[i][0], samples[i][1], sqrt(samples[i][0]*samples[i][0] + samples[i][1]*samples[i][1]));
	
		fclose(fft_utan_fp);
		printf(" done.\n");
		printf("plotting to fft_utan.pdf ...");
		if(system("gnuplot pdfplotter.gp"))
			printf("gnuplot error ...\n");
		else
			printf(" done.\n");
	}

	fftwf_free(samples);
    //fftwf_free(in); fftwf_free(out);
	return 0;
}
