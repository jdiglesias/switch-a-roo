
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
	}

	void setTimeSlices(std::list<int> ts){
		timeSlices = new std::list<int>(ts);
	}

	int sampleRate;
	std::list<int> * timeSlices;
	int64 totalNumSamples;
	double timeLen;
	const float * samples;
	int thresh;
	int minSlice;
};