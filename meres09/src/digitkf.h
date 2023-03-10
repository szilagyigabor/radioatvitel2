#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>

// (*a)=(*a)*(*b)
void complex_mul(fftwf_complex *a, fftwf_complex *b);

// plot an array of complex numbers with gnuplot
void plot(float *xdata, fftwf_complex *arr, int length, int stride, const char *name, const char *title);

void normalize(fftwf_complex *arr, int N);
