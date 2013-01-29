#ifndef FFT_H_
#define FFT_H_

/* constant declarations */
#define PI 3.14159265358979
#define LOG2 0.6931471805599453
#include <vector>

class FFT
{
	public:
		FFT(int dataSize);
		//FFT(int dataSize):n(dataSize);
		void fft(double *x, double *y);
		std::vector<float> windowed_fft(float *buffer,int bufferStartPos,int bufferSize);
		virtual ~FFT();
		double *sinx;
		double *cosx;
	private:
		int n;
		int m;
		double *window;
};

#endif /*FFT_H_*/
