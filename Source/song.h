#include <assert.h>
class songProperties{
public:


	songProperties(int sampRate,
		int64 totNumSamples,
		double tLen,
		const float * samps,
		int newthresh,
		int newminSlic)
	{
		sampleRate = sampRate;
		totalNumSamples = totNumSamples;
		timeLen = tLen;
		samples =samps;
		thresh =newthresh;
		minSlice =newminSlic;

		getSlicesByAmplitude(samps, totalNumSamples, thresh, minSlice);
	}

	void setTimeSlices(std::list<int> ts){
		timeSlices = new std::list<int>(ts);
	}

	void setZerosAndPeaks(const float signal[], const int length){
		float currentPeakValue = signal[0];
		int sign = signal[0] > 0 ? 1 : -1;
		for (int i = 0; i < length; i++){
			if (signal[i] * sign <= 0){
				sign *= -1;
				zerosAndPeaks.push_back(zeroAndPeak_t{ i, currentPeakValue });
			}
			else if (std::abs(signal[i]) > currentPeakValue){
				currentPeakValue = std::abs(signal[i]);
			}
		}
	}
	/*
	* getSlices returns a list of slices into the sound. the slices are based on jumps in amplitude in the sound,
	* large enough jumps will result in a slice at the beginning of the jump. large enough is determined by the threshold parameter
	*/
	std::list<int>& getSlicesByAmplitude(const float signal[], const int length,
		const float threshold, int minSliceLen){

		if (zerosAndPeaks.empty()){
			setZerosAndPeaks(signal, length);
		}
		//std::list<int> zeroesAndPeaks = zerosAndPeaksGlobal;
		static std::list<int> slices = std::list<int>();
		slices.push_back(0);
		int sign = zerosAndPeaks[0].peak > zerosAndPeaks[1].peak ? 1 : -1;
		int slicePoint = 0;
		int averagedChunk = 6;

		float average;
		std::vector<zeroAndPeak_t>::iterator i = zerosAndPeaks.begin();
		std::vector<zeroAndPeak_t>::iterator lastZeroAndPeak = zerosAndPeaks.begin();
		/*for(i = zerosAndPeaks.begin(); i->zero != zerosAndPeaks[averagedChunk].zero && i != zerosAndPeaks.end(); i++){
		}*/
		std::vector<zeroAndPeak_t>::iterator end = zerosAndPeaks.end();

		while (i != zerosAndPeaks.end()){
			if (std::abs(i->peak - lastZeroAndPeak->peak) * sign < 0){
				if (sign == 1){
					slicePoint = lastZeroAndPeak->zero;
					sign *= -1;
				}
			}
			if (std::abs(i->peak - lastZeroAndPeak->peak) > threshold && i->zero - slices.back() > minSliceLen){
				slices.push_back(i->zero);
			}
			lastZeroAndPeak = i;
			i++;
		}
		return slices;
	}

	std::list<int>& getSlicesByFrequency(const float signal[], const int length, const float periodThreshold, const float varianceThreshold, const int minSliceLen){
		if (zerosAndPeaks.empty()){
			setZerosAndPeaks(signal, length);
		}
		//std::list<int> zeroesAndPeaks = zerosAndPeaksGlobal;
		static std::list<int> slices = std::list<int>();
		int averagedChunkLen = 30;
		float currentVariance = 0;
		float currentPeriod = 0;
		/*This code uses previousPeriods to figure out if the current behavior of the waveform is periodic.
		Periodic is defined rather naively as crossing the x-axis at regular enough intervals.*/
		std::list<float> previousPeriods;
		float previousPeriodsSum = 0;
		float zeroCrossingRate = 0;
		float lastZeroCrossingRate = 0;
		std::vector<zeroAndPeak_t>::iterator i = zerosAndPeaks.begin();
		std::vector<zeroAndPeak_t>::iterator  lastZeroAndPeak = zerosAndPeaks.begin();
		bool lookingForSignal = true;
		bool populatingPreviousPeriods = true;
		for (i = zerosAndPeaks.begin(); i != zerosAndPeaks.end(); i++){
			if (i == zerosAndPeaks.begin()){
				lastZeroAndPeak = i;
			}
			else{
				currentPeriod = i->zero - lastZeroAndPeak->zero;

				if (populatingPreviousPeriods){
					assert(previousPeriods.size() < averagedChunkLen);
					previousPeriodsSum += currentPeriod;
					previousPeriods.push_back(currentPeriod);

					if (previousPeriods.size() == averagedChunkLen){
						populatingPreviousPeriods = false;
						zeroCrossingRate = previousPeriodsSum / averagedChunkLen;
						currentVariance = 0;
						for (std::list<float>::iterator p = previousPeriods.begin(); p != previousPeriods.end(); p++){
							assert(zeroCrossingRate > 0);
							currentVariance += std::abs(*p - zeroCrossingRate) / zeroCrossingRate;
						}
						if (currentVariance < varianceThreshold &&
							(slices.empty() || (i - averagedChunkLen)->zero - slices.back() > minSliceLen) &&
							(lastZeroCrossingRate == 0 || std::abs(lastZeroCrossingRate - zeroCrossingRate) > periodThreshold)){
							slices.push_back((i - averagedChunkLen)->zero);
						}
					}
				}

				else{
					assert(previousPeriods.size() == averagedChunkLen);
					if (std::abs(currentPeriod - zeroCrossingRate) > periodThreshold && !lookingForSignal){
						previousPeriodsSum = currentPeriod;
						previousPeriods.clear();
						previousPeriods.push_back(currentPeriod);
						lastZeroCrossingRate = zeroCrossingRate;
						populatingPreviousPeriods = true;
						continue;
					}

					previousPeriodsSum -= previousPeriods.front();
					previousPeriods.pop_front();
					previousPeriodsSum += currentPeriod;
					previousPeriods.push_back(currentPeriod);
					zeroCrossingRate = previousPeriodsSum / averagedChunkLen;
					currentVariance = 0;
					for (std::list<float>::iterator p = previousPeriods.begin(); p != previousPeriods.end(); p++){
						assert(zeroCrossingRate > 0);
						currentVariance += std::abs(*p - zeroCrossingRate) / zeroCrossingRate;
					}

					if (lookingForSignal){
						if (currentVariance < varianceThreshold){ //have we found a signal?
							if (slices.empty() || i->zero - slices.back() > minSliceLen){
								slices.push_back((i - averagedChunkLen)->zero);
							}
							lookingForSignal = false;
						}
					}
					else{
						if (currentVariance > varianceThreshold){ //have we lost the signal?
							lookingForSignal = true;
							slices.push_back((i - averagedChunkLen)->zero);
						}
					}
				}
			}
		}

		return slices;
	}

	const int nearestZero(const int index){
		int lastZero = 0;
		try{
			if (zerosAndPeaks.empty()){
				throw;
			}
		}
		catch (int e){
			std::cerr << "peaksAndZeroes is empty." << std::endl;
			return 0;
		}

		for (std::vector<zeroAndPeak_t>::iterator i = zerosAndPeaks.begin(); i != zerosAndPeaks.end(); i++){
			jassert(i->zero > lastZero);
			if (i->zero > index){
				if (index - lastZero < i->zero - index){
					return lastZero;
				}
				else{
					return i->zero;
				}
			}
			lastZero = i->zero;
		}
		return lastZero;
	}

	typedef struct zeroAndPeak{
		int zero;
		float peak;
	} zeroAndPeak_t;
	int sampleRate;
	std::list<int> * timeSlices;
	int64 totalNumSamples;
	double timeLen;
	const float * samples;
	int thresh;
	int minSlice;
	std::vector<zeroAndPeak_t> zerosAndPeaks = std::vector<zeroAndPeak_t>();

};