#include<complex>
using namespace std;

class fftContainer{
public:
	fftContainer(int len, float fund, float * amps, complex<float> * rawDa, float * rawAud){
		length = len;
		fundamental = fund;
		amplitudes = amps;
		rawFFT = rawDa;
		rawAudio = rawAud;
	}

	int length;
	float fundamental;
	float * amplitudes;
	complex<float> * rawFFT;
	float * rawAudio;
};