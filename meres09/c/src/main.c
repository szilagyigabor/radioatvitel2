#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>
#include <digitkf.h>


#define FILENAME "../noisy.cf32"
#define SAMPLE_FREQ 4000000.0f
#define CARRIER_FREQ 1440000.0f
#define DATA_RATE 40000.0f
#define PI 3.1415926535897932384626433832795f
#define CODE_LENGTH 128
#define PLOT_REAL 1
#define PLOT_IMAG 2
#define PLOT_REAL_IMAG 3
#define PLOT_ABS 4
#define PLOT_REAL_ABS 5
#define PLOT_IMAG_ABS 6
#define PLOT_REAL_IMAG_ABS 7

int main(void)
{
    int code_int[] = {3, 2, 2, 2, 1, 2, 3, 0, 3, 3, 0, 3, 2, 0, 1, 3, 0, 2, 3, 1, 0, 1, 2, 3, 2, 1, 1, 1, 1, 2, 0, 3, 2, 2, 3, 3, 3, 1, 3, 0, 2, 1, 2, 3, 2, 2, 2, 2, 2, 0, 3, 1, 2, 0, 0, 3, 1, 3, 2, 3, 0, 3, 2, 2, 1, 1, 2, 0, 2, 1, 1, 0, 1, 0, 3, 2, 2, 2, 1, 0, 1, 3, 2, 2, 0, 3, 2, 2, 3, 1, 1, 2, 2, 1, 3, 2, 0, 3, 1, 1, 3, 0, 3, 1, 2, 1, 3, 2, 2, 3, 2, 3, 2, 1, 2, 0, 0, 3, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1};
    fftwf_complex *code = malloc(sizeof(fftwf_complex)*CODE_LENGTH);
    for(int i=0; i<CODE_LENGTH; i++)
    {
        switch(code_int[i])
        {
            case 0:
                code[i][0] = 1.0f;
                code[i][1] = 0.0f;
                break;
            case 1:
                code[i][0] = 0.0f;
                code[i][1] = -1.0f;
                break;
            case 2:
                code[i][0] = -1.0f;
                code[i][1] = 0.0f;
                break;
            case 3:
                code[i][0] = 0.0f;
                code[i][1] = 1.0f;
                break;
        }
        //printf("%d -> %+01.0f%+01.0fi\n",code_int[i],code[i][0],code[i][1]);
    }


    // open file and determine number of complex samples
    FILE* noisy_file = fopen(FILENAME,"r");
    
    fseek(noisy_file, 0, SEEK_END);
    int N = ftell(noisy_file)/8;
    fseek(noisy_file, 0, SEEK_SET);
    
    fftwf_complex *samples = fftwf_malloc(sizeof(fftwf_complex) * N);
    fftwf_complex *spectrum = fftwf_malloc(sizeof(fftwf_complex) * N);
    size_t RET_CODE = fread(samples, sizeof(fftwf_complex), N, noisy_file);
    if(RET_CODE == N)
        printf("%s read successfully\n", FILENAME);
    else
        printf("error while reading %s\n", FILENAME);

    fclose(noisy_file);

    fftwf_plan pf, pb;
    pf = fftwf_plan_dft_1d(N, samples, spectrum, FFTW_FORWARD, FFTW_ESTIMATE);
    pb = fftwf_plan_dft_1d(N, spectrum, samples, FFTW_BACKWARD, FFTW_ESTIMATE);
    
    // frequency vector used for plotting
    float *freq_N;
    freq_N = malloc(sizeof(float)*N);
    float freq_step = 1.0f/(float)N*SAMPLE_FREQ/1000000.0f;
    //float freq_step = 1.0f/(float)N*SAMPLE_FREQ;
    float freq_stepper=0.0f;
    for(int i=0; i<N; i++)
    {
        freq_N[i] = freq_stepper;
        freq_stepper += freq_step;
    }

    // time vector used for plotting
    float *time_N;
    time_N = malloc(sizeof(float)*N);
    float time_step = 1.0f/SAMPLE_FREQ;
    //float freq_step = 1.0f/(float)N*SAMPLE_FREQ;
    float time_stepper=0.0f;
    for(int i=0; i<N; i++)
    {
        time_N[i] = time_stepper;
        time_stepper += time_step;
    }
    
    // print original time domain data
    plot(time_N, samples, N/512, 2, "original_samples", "original samples", PLOT_REAL_IMAG);

    // calculate and print original spectrum
       fftwf_execute(pf);
    normalize(spectrum, N);
    plot(freq_N, spectrum, N, 1024, "original_spectrum", "original spectrum", PLOT_REAL_IMAG);

    // mix from the carrier to DC
    float dfi = 2.0f*PI/SAMPLE_FREQ*CARRIER_FREQ;
    fftwf_complex mixer, rotator;
    rotator[0] = cos(dfi);
    rotator[1] = -sin(dfi);
    mixer[0] = 1.0f;
    mixer[1] = 0.0f;
    for(int i=0; i<N; i++)
    {
        complex_mul(&mixer, &rotator);
        complex_mul(&(samples[i]), &mixer);
    }

    // calculate mixed spectrum
    fftwf_execute(pf);
    
    // print mixed spectrum
    plot(freq_N, spectrum, N, 1024, "mixed_spectrum", "mixed spectrum", PLOT_REAL_IMAG);

    // brute force low pass filter
        // calculate first and last index to be zeroed
    int first = (int)(40000.0f*N/SAMPLE_FREQ);
    int last = N-first;
    for(int i=first; i<last+1; i++)
    {
        spectrum[i][0] = 0.0;
        spectrum[i][1] = 0.0;
    }

    // print filtered spectrum
    plot(freq_N, spectrum, N, 1024, "filtered_spectrum", "filtered spectrum", PLOT_REAL_IMAG);
    
    // print filtered time domain data
    fftwf_execute(pb);
    normalize(samples, N);
    plot(time_N, samples, N/512, 2, "filtered_samples", "filtered samples", PLOT_REAL_IMAG);

    // decimate time domain data
        // calculate decimation factor so that one spreading symbol lasts 16 samples
    int factor = (int)(SAMPLE_FREQ/DATA_RATE);
    printf("dec factor: %d\n",factor);
    int n = N/factor;
    fftwf_complex *decimated_samples = fftwf_malloc(sizeof(fftwf_complex)*n);
    fftwf_complex *correlated_samples = fftwf_malloc(sizeof(fftwf_complex)*n);
    fftwf_complex *decimated_spectrum = fftwf_malloc(sizeof(fftwf_complex)*n);
    
    // averaging decimation
    fftwf_complex sum;
    for(int i=0; i<n; i++)
    {
        sum[0]=0.0f;
        sum[1]=0.0f;
        for(int j=0; j<factor; j++)
        {
            sum[0] += samples[i*factor+j][0];
            sum[1] += samples[i*factor+j][1];
        }
        decimated_samples[i][0] = sum[0]/(float)factor;
        decimated_samples[i][1] = sum[1]/(float)factor;
    }
    
    // time vector used for plotting
    float *time_decimated;
    time_decimated = malloc(sizeof(float)*n);
    time_step = 1.0f/SAMPLE_FREQ*(float)factor;
    //float freq_step = 1.0f/(float)N*SAMPLE_FREQ;
    time_stepper=0.0f;
    for(int i=0; i<n; i++)
    {
        time_decimated[i] = time_stepper;
        time_stepper += time_step;
    }

    // plot decimated time domain data
    plot(time_decimated, decimated_samples, N/100/factor, 1, "decimated_samples", "decimated samples", PLOT_REAL_IMAG);

    //correlate with the spreading code
    for(int i=0; i<n-CODE_LENGTH; i++)
    {
        correlated_samples[i][0] = 0.0f;
        correlated_samples[i][1] = 0.0f;
        for(int j=0; j<CODE_LENGTH; j++)
        {
            correlated_samples[i][0] += decimated_samples[i+j][0]*code[j][0] - decimated_samples[i+j][1]*code[j][1];
            correlated_samples[i][1] += decimated_samples[i+j][0]*code[j][1] + decimated_samples[i+j][1]*code[j][0];
        }
    }

    // plot correlation
    plot(time_decimated, correlated_samples, n, 1, "correlated_samples", "correlated samples", PLOT_REAL_IMAG);

    free(code);
    free(time_decimated);
    fftwf_free(decimated_samples);
    fftwf_free(correlated_samples);
    fftwf_free(decimated_spectrum);
    free(freq_N);
    free(time_N);
    fftwf_destroy_plan(pf);
    fftwf_destroy_plan(pb);
    fftwf_free(samples);
    fftwf_free(spectrum);
    return 0;
}
