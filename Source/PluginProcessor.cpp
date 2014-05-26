//test line for git ignore
/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
/*extern "C"{
	#include "kiss_fft130\kiss_fft.h"
}*/
#include <stdlib.h>
#include <list>
#include <cmath>
#include <complex>

#include <utility> //for slice pairs std::pair
//==============================================================================


SwitcharooAudioProcessor::SwitcharooAudioProcessor()
{
	UserParams[MasterBypass] = 0.0f;//default to not bypassed
		//repeat for "OtherParams"
	UIUpdateFlag = true;//Request UI update
}

//int SwitcharooAudioProcessor::getNumParameters(){ return totalNumParam; }
SwitcharooAudioProcessor::~SwitcharooAudioProcessor()
{
}
// custom code
AudioSampleBuffer * SwitcharooAudioProcessor::readFile(File compareFile, AudioFormatManager* format){
	AudioFormatReader* compareReader = format->createReaderFor(compareFile);
	AudioSampleBuffer * compareBuffer;
	compareReader->read(compareBuffer, 0, compareReader->lengthInSamples, 0, true, true);
	return compareBuffer;
}
AudioSampleBuffer * SwitcharooAudioProcessor::processFile(File compareFile){
	AudioFormatManager* format = new AudioFormatManager();
	format->registerBasicFormats();
//	SwitcharooAudioProcessorEditor * currentEditor = getEditor();
	AudioSampleBuffer * compareBuffer = readFile(compareFile, format);
	//std::vector<int> slices = getSlices(compareBuffer);
	//currentEditor->setupThumb(format);
	return compareBuffer;
}

//==============================================================================
const String SwitcharooAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int SwitcharooAudioProcessor::getNumParameters()
{
    return totalNumParam;
}

float SwitcharooAudioProcessor::getParameter (int index)
{
	if (index >= 0 && index < totalNumParam)
		return UserParams[index];
	else
		return 0.0f;//invalid index
}

void SwitcharooAudioProcessor::setParameter (int index, float newValue)
{
	if (index >= 0 && index<totalNumParam)
		UserParams[index] = newValue;
}

const String SwitcharooAudioProcessor::getParameterName (int index)
{
	switch (index)
	{
	case MasterBypass: return "Master Bypass";
		//OtherParams...
	default:return String::empty;
	}
}

const String SwitcharooAudioProcessor::getParameterText (int index)
{
	if (index >= 0 && index<totalNumParam)
		return String(UserParams[index]);//return parameter value as string
	else return String::empty;
}

/*
 * getInterleavedZeroesAndPeaks is called by the slicer which makes slices on zeroes,
 * depending on whether the difference between the previous peak and the current one is large enough
 * remember, you get a zero first, then a peak.
 */
 const std::list<int>& SwitcharooAudioProcessor::getInterleavedZeroesAndPeaks(const float signal[], const int length){
    int currentPeakIndex = 0;
    float currentPeakValue = signal[0];
    int sign = signal[0] >= 0 ? 1 : -1; 
    static std::list<int> zeroesAndPeaks = std::list<int>();
    for(int i = 0; i < length; i++){
        if(signal[i] * sign <= 0){
            zeroesAndPeaks.push_back(i);
            zeroesAndPeaks.push_back(currentPeakIndex);
            sign *= -1;
            currentPeakValue = std::abs(signal[i]);
        }
        else{
            if(std::abs(signal[i]) > currentPeakValue){
                currentPeakValue = std::abs(signal[i]);
                currentPeakIndex = i;
            } 
        }
    }
    return zeroesAndPeaks;
}
/*
 * getSlices returns a list of slices into the sound. the slices are based on jumps in amplitude in the sound, 
 * large enough jumps will result in a slice at the beginning of the jump. large enough is determined by the threshold parameter
 */            
const std::list<int>& SwitcharooAudioProcessor::getSlicesByAmplitude(const float signal[],const int length, const float threshold){
	if (zerosAndPeaksGlobal.empty()){
		zerosAndPeaksGlobal = getInterleavedZeroesAndPeaks(signal, length);

	}
	std::list<int> zeroesAndPeaks = getInterleavedZeroesAndPeaks(signal, length);
	static std::list<int> slices = std::list<int>();
    int currentZero;
    float lastPeak = 0;
    for(std::list<int>::iterator i = zeroesAndPeaks.begin(); i != zeroesAndPeaks.end(); i++){
        currentZero = *i;
        i++;
        if(abs(signal[*i] - lastPeak) > threshold){
            slices.push_back(currentZero);
        }
        lastPeak = signal[*i];
    }
    return slices;
    
}
const File SwitcharooAudioProcessor::loadFile()
{
    FileChooser chooser ("Please select the file you want to load...",
                           File::getSpecialLocation (File::userHomeDirectory));
    if (chooser.browseForFileToOpen())
    {
        File file (chooser.getResult());
        return file;
    }
    return File();
}

const String SwitcharooAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String SwitcharooAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool SwitcharooAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool SwitcharooAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool SwitcharooAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SwitcharooAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SwitcharooAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double SwitcharooAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SwitcharooAudioProcessor::getNumPrograms()
{
    return 0;
}

int SwitcharooAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SwitcharooAudioProcessor::setCurrentProgram (int index)
{
}

const String SwitcharooAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void SwitcharooAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SwitcharooAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SwitcharooAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void SwitcharooAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	//processAudioBuffer(buffer);
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
   /* File compareFile = loadFile();
    float* channelData;
    float* sampleData;
    int sampleNum;
//    AudioFormatManager format;
    AudioFormatManager* format = new AudioFormatManager();
    format->registerBasicFormats();
    AudioFormatReader* compareReader = format->createReaderFor(compareFile);
    AudioSampleBuffer * compareBuffer;
    compareReader->read(compareBuffer, 0, compareReader->lengthInSamples, 0, true, true);
    std::list<std::vector<int> > segmentIndexes;
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        std::list<std::vector<int> >::iterator segments = segmentIndexes.begin();
        channelData = buffer.getSampleData (channel);
        sampleData = compareBuffer->getSampleData(channel);
        sampleNum = buffer.getNumSamples();
        segmentIndexes.push_back(getSegments(channelData,sampleNum));
        for(std::vector<int>::iterator segment = segments->begin(); segment != segments->end(); segment++){
            if(segment+1 != segments->end()){
                compareSamples(channelData,sampleData,*(segment+1) - *segment, compareBuffer->getNumSamples(), *segment );
            }
        }
        segments++;
    }
	*/
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
//    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
  //  {
    //    buffer.clear (i, 0, buffer.getNumSamples());
    //}
}
/*
std::vector<std::pair<int,int> > SwitcharooAudioProcessor::getSlices(float *channelData){
	//this does slicing by volume of some kind.
	std::vector<std::pair<int, int> > ret;

	return ret;
}

std::vector<int> SwitcharooAudioProcessor::getSegments(float* channelData, int length){
    std::vector<int> returnSegments;
    for(int i= 0; i<length-1; i+=getSampleRate()) returnSegments.push_back(i);
    returnSegments.push_back(length-1);
    return returnSegments;
}
void SwitcharooAudioProcessor::compareSamples(float* sourceData, float* sampleData, int sourceLength, int sampleLength, int offset){
    int sampleIndex, comparisonValue, bestComparisonValue, bestComparisonStart;    
    for(int sampleStart = 0; sampleStart<sampleLength-sourceLength; sampleStart++){
        for(int sourceIndex = offset; sourceIndex<offset+sourceLength; sourceIndex++){
            sampleIndex = sampleStart+sourceIndex-offset;
            comparisonValue += abs(sourceData[sourceIndex] - sampleData[sampleIndex]);
        }
        if(comparisonValue < bestComparisonValue){
            bestComparisonValue = comparisonValue;
            bestComparisonStart = sampleStart;
        }       
    }
    for(int i = bestComparisonStart; i< bestComparisonStart+sourceLength; i++){
        sourceData[offset + i-bestComparisonStart] = sampleData[i];
    }
}
            
*/        


        
//==============================================================================
bool SwitcharooAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SwitcharooAudioProcessor::createEditor()
{
    return new SwitcharooAudioProcessorEditor (this);
}

//==============================================================================
void SwitcharooAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SwitcharooAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


void SwitcharooAudioProcessor::processAudioBuffer(AudioSampleBuffer& buffer){
	testbuf = buffer;
	num_blocks++;
}


float * SwitcharooAudioProcessor::doFFT(const float signal[], int signalLength){
	//setting up this cfg will optimize the speed at which this gets processed.
	kiss_fftr_cfg cfg = kiss_fftr_alloc(signalLength * 2,0,NULL,NULL);
//might need to change bc this may not be big enough
	//kiss_fft_scalar fin[100000];
	kiss_fft_scalar * fin = new kiss_fft_scalar[signalLength];
	//kiss_fft_cpx * fout = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx) * (signalLength/2 +1))
		kiss_fft_cpx * fout =	new kiss_fft_cpx[signalLength / 2 + 1];
	//kiss_fft_cpx fout[50001];
	//std::complex<float> * out = (std::complex<float> *) malloc(sizeof(std::complex<float>) * (signalLength/2 +1));
	for (int j = 0; j < signalLength; j++){
		fin[j] = signal[j];
	}

	kiss_fftr(cfg, fin, fout);
	float * fftSignal= new float[signalLength/2 +1]();

//	float fftSignal[1024];
	// only show the real signal that was returned... not the imaginary.
	int real = 0;
	for (int k = 0; k < (signalLength/2)+1; k++){
		if (fout[k].r < 0){
			real = fout[k].r * -1;
		}else {
			real = fout[k].r;
		}
		fftSignal[k] = sqrt(real) +(fout[k].i * fout[k].i);
	}

	//delete(&cfg);

	return fftSignal;

}
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SwitcharooAudioProcessor();
}
