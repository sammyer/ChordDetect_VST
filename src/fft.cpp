#include "fft.h"
#include <cmath>
#include <exception>
#include <cstdio>
#include <vector>

/* function declarations */

FFT::FFT(int dataSize):n(dataSize) {
	//ctor
		int i;

		//m=log2(n)
		m = 0;
		int n2=n;
		while (n2>1) {
			n2>>=1;
			m+=1;
		}
		// Make sure n is a power of 2
		//if (n != (1 << m)) throw std::bad_alloc;

		// precompute tables
		cosx = new double[n / 2];
		sinx = new double[n / 2];
		window = new double[n];

		for (i = 0; i < n / 2; i++) {
			cosx[i] = cos(-2 * PI * i / n);
			sinx[i] = sin(-2 * PI * i / n);
		}
		for (i=0;i<n;i++) window[i] = 0.5*(1.0+sin(2*PI*i/n));
}

FFT::~FFT() {
	//dtor
	delete[] cosx ;
	delete[] sinx;
	delete[] window;
}

std::vector<float> FFT::windowed_fft(float *buffer,int bufferStartPos,int bufferSize) {
	//output must be n/2 size
	double *x=new double[n];
	double *y=new double[n];
	std::vector<float> output;
	output.resize(n/2+1);
	int i;
	for (i=0;i<n;i++) {
		x[i]=buffer[(bufferStartPos+i)%bufferSize]*window[i]/n*2;
		y[i]=0.0;
	}
	fft(x,y);
	for (i=0;i<n/2+1;i++) {
		output[i]=(float)sqrt(x[i]*x[i]+y[i]*y[i]);
	}
	delete[] x;
	delete[] y;
	return output;
}

void FFT::fft(double *x, double *y) {
	int i, j, k, n1, n2, a;
	double c, s, t1, t2;

	// Bit-reverse
	j = 0;
	n2 = n / 2;
	for (i = 1; i < n - 1; i++) {
		n1 = n2;
		while (j >= n1) {
			j = j - n1;
			n1 = n1 / 2;
		}
		j = j + n1;

		if (i < j) {
			t1=x[i]; x[i]=x[j]; x[j]=t1; //swap
			t1=y[i]; y[i]=y[j]; y[j]=t1; //swap
		}
	}

	// FFT
	n1 = 0;
	n2 = 1;

	for (i = 0; i < m; i++) {
		n1 = n2;
		n2 = n2 + n2;
		a = 0;

		for (j = 0; j < n1; j++) {
			c = cosx[a];
			s = sinx[a];
			a += (1 << (m - i - 1));

			for (k = j; k < n; k = k + n2) {
				t1 = c * x[k + n1] - s * y[k + n1];
				t2 = s * x[k + n1] + c * y[k + n1];
				x[k + n1] = x[k] - t1;
				y[k + n1] = y[k] - t2;
				x[k] = x[k] + t1;
				y[k] = y[k] + t2;
			}
		}
	}
}
