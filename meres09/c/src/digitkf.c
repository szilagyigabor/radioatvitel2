#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>
#include <digitkf.h>

// (*a)=(*a)*(*b)
void complex_mul(fftwf_complex *a, fftwf_complex *b)
{
	float im = (*a)[0]*(*b)[1] + (*a)[1]*(*b)[0];
	float re = (*a)[0]*(*b)[0] - (*a)[1]*(*b)[1];
	(*a)[0] = re;
	(*a)[1] = im;
	return;
}
//	fftwf_complex a, b;
//	a[0]=1.0f;
//	a[1]=2.0f;
//	b[0]=3.0f;
//	b[1]=4.0f;
//	printf("a=%f+%fi b=%f+%fi\n",a[0],a[1],b[0],b[1]);
//	complex_mul(&a, &b);
//	printf("a=%f+%fi b=%f+%fi\n",a[0],a[1],b[0],b[1]);


// plot an array of complex numbers with gnuplot
void plot(float *xdata, fftwf_complex *arr, int length, int stride, const char *name, const char *title, int select)
{
    if(select<1 || select>7)
    {
        printf("wrong plot selection\n");
        return;
    }

	FILE *fs = fopen("tmp/samples.txt","w");
	fprintf(fs, "#serial real imag abs\n");
	for(int i=0; i<length; i+=stride)
		fprintf(fs, "%f %f %f %f\n", xdata[i], arr[i][0], arr[i][1], sqrt(arr[i][0]*arr[i][0] + arr[i][1]*arr[i][1]));
	fclose(fs);
	
    FILE *fp = fopen("tmp/plotter.gp", "w");
    fprintf(fp, "set terminal pdfcairo\n");
    fprintf(fp, "set output \"output/%s.pdf\"\n", name);
    fprintf(fp, "set title \"%s\"\n", title);
    fprintf(fp, "set xr [%f:%f]\n", (xdata[0]-xdata[length-1])*0.05f, xdata[length-1]*1.05f);
    
    if(select&1)
    {
        fprintf(fp, "plot \"tmp/samples.txt\" u 1:2 t \"Real\" w l,\\\n");
    }

    if(select&2)
    {
        if(select&1)
        {
            fprintf(fp, "     \"tmp/samples.txt\" u 1:3 t \"Imag\" w l,\\\n");
        }
        else
        {
            fprintf(fp, "plot \"tmp/samples.txt\" u 1:3 t \"Imag\" w l,\\\n");
        }
    }

    if(select&4)
    {
        if(select&1 || select&2)
        {
            fprintf(fp, "     \"tmp/samples.txt\" u 1:4 t \"Abs\" w l");
        }
        else
        {
            fprintf(fp, "plot \"tmp/samples.txt\" u 1:4 t \"Abs\" w l");
        }
    }

    fclose(fp);
    
	if(system("gnuplot tmp/plotter.gp"))
		printf("gnuplot error ...\n");
    
    return;
}

// normalize time of frequency domain data after (I)FFT
void normalize(fftwf_complex *arr, int N)
{
    float Nf = (float)N;
    for(int i=0; i<N; i++)
    {
        arr[i][0] = arr[i][0]/Nf;
        arr[i][1] = arr[i][1]/Nf;
    }
    return;
}
