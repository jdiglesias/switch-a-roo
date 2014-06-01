#include <complex>
#include "kiss_fft130/tools/kiss_fftr.h"
#include "kiss_fft130/tools/kiss_fft.h"


class fftContainer{
public:
	fftContainer(int len, float fund, float * amps, std::complex<float> * rawDa, float * rawAud, int rawAudLen){
		length = len;
		fundamental = fund;
		amplitudes = amps;
		rawFFT = rawDa;
		rawAudio = rawAud;
		rawAudioLength = rawAudLen;
	}

	int length;
	float fundamental;
	float * amplitudes;
	std::complex<float> * rawFFT;
	float * rawAudio;
	int rawAudioLength;
	/*
	fftContainer * doFFT(const float * signal, int signalLength){
		int nextPow2 = 1;
		//read in musi-math need for optimization
		while (nextPow2 < signalLength) {
			nextPow2 *= 2;
		}

		kiss_fftr_cfg cfg = kiss_fftr_alloc(nextPow2, 0, NULL, NULL);
		kiss_fft_scalar * fin = new kiss_fft_scalar[nextPow2];
		kiss_fft_cpx * fout = new kiss_fft_cpx[(nextPow2 / 2) + 1];

		//ignore first sample might be header data
		for (int j = 1; j < signalLength; j++){
			fin[j] = signal[j];
		}
		//pad out next part to make the signal a power of two.
		int temp_input = signalLength + 1;
		while (temp_input < nextPow2){
			fin[temp_input] = 0;
			temp_input++;
		}

		float * fftSignal = new float[(nextPow2 / 2) + 1];

		std::complex<float> * real_signal = new std::complex<float>[(nextPow2 / 2) + 1];
		kiss_fftr(cfg, fin, fout);
		float fund = 0.0;
		float cur_real, cur_img;
		float real = 0;
		for (int k = 0; k < (nextPow2 / 2) + 1; k++){
			cur_real = fout[k].r / (nextPow2 / 2);
			cur_img = fout[k].i / (nextPow2 / 2);

			real = (cur_real * cur_real) + (cur_img * cur_img);
			fftSignal[k] = sqrt(real);
			if (fund < sqrt(real)){
				fund = sqrt(real);
			}
			real_signal[k] = std::complex<float>(cur_real, cur_img);
		}
		//fftContainer retFFT((nextPow2 / 2) + 1, fund, fftSignal, real_signal);
		fftContainer * retFFT = new fftContainer((nextPow2 / 2) + 1, fund, fftSignal, real_signal, fin);
		free(cfg);
		return retFFT;
	}

	std::vector<fftContainer> * doFFTtoSlices(std::list<int> timeslices, const float signal[], int signalLength){
		//for time slices
		std::vector<fftContainer> * fftVector = new std::vector<fftContainer>;
		fftContainer * cur_slice;
		const float * sig;
		int time_slice_loop = 0;
		int prev_slice;
		int last_slice;
		int cur_slice_len;
		float * sigStart;
		for (std::list<int>::iterator it = timeslices.begin(); it != timeslices.end(); it++){
			//skip 0 slice
			if (*it != 0){

				//sets length of slice
				if (time_slice_loop == 0){
					prev_slice = 0;
				}
				else{
					it--;
					prev_slice = *it;
					it++;
				}
				cur_slice_len = *it - prev_slice;
				sigStart = new float[cur_slice_len];
				int i = prev_slice;
				int index = 0;
				for (i; i < *it; i++){
					sigStart[index] = signal[i];
					index++;
				}

				cur_slice = doFFT(sigStart, cur_slice_len);
				//dereferencing should be ok.
				fftVector->push_back(*cur_slice);
				time_slice_loop++;
			}
			last_slice = *it;
		}

		sigStart = new float[signalLength - last_slice];
		int j = last_slice;
		int index = 0;
		for (j; j < signalLength; j++){
			sigStart[index] = signal[j];
			index++;
		}
		cur_slice_len = signalLength - last_slice;
		cur_slice = doFFT(sigStart, cur_slice_len);
		fftVector->push_back(*cur_slice);
		//fftVector.push_back(retFFT);
		return fftVector;
	}
	*/
};