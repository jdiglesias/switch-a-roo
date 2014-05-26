//test line for git ignore
/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdlib.h>
#include <list>
#include <cmath>
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
 * setZerosAndPeaks finds roughly where there are zero values in the waveform but tries to cancel out noise 
 * by looking at the average of a chunk of values rather than one value at a time. It
 */
void SwitcharooAudioProcessor::setZerosAndPeaks(const float signal[], const int length, int averagedChunk){
     int currentPeakIndex = 0;
     float currentPeakValue = signal[0];
     float average = 0;
     int realign = averagedChunk / 2;
     for(int i = 0; i < averagedChunk && i<length; i++){
         average+=signal[i];
     }
     int sign = average > 0 ? 1 : -1;
     for(int i = averagedChunk; i < length; i++){
        average += signal[i] - signal[i-averagedChunk];
        if((average/averagedChunk) * sign <= 0){

            sign *= -1;
            currentPeakValue = std::abs(signal[i-realign]);
            //Move the loop forward without checking to see if the sign changes after each iteration, but still updating peak info
            while(realign > 0 && i < length-1){
                i++;
                average += signal[i] - signal[i-averagedChunk];
                if(std::abs(signal[i-averagedChunk]) > currentPeakValue){
                    currentPeakValue = std::abs(signal[i-averagedChunk]);
                    currentPeakIndex = i-averagedChunk;
                }
                realign--;
            }
            realign = averagedChunk / 2;
            //Now we check if the sign changed during realignment. if not, we can commit our zero and peak.
            if((average/averagedChunk) * sign > 0){
                zerosAndPeaks.push_back(zeroAndPeak_t{i - averagedChunk, currentPeakValue});
            }
            else{
                sign *= -1;
            }
        }
        else{
            if(std::abs(signal[i-averagedChunk]) > currentPeakValue){
                currentPeakValue = std::abs(signal[i-averagedChunk]);
                currentPeakIndex = i-averagedChunk;
            }
        }
    }
}
/*
 * getSlices returns a list of slices into the sound. the slices are based on jumps in amplitude in the sound, 
 * large enough jumps will result in a slice at the beginning of the jump. large enough is determined by the threshold parameter
 */            
const std::list<int>& SwitcharooAudioProcessor::getSlicesByAmplitude(const float signal[],const int length, const float threshold, int minSliceLen){
	if (zerosAndPeaks.empty()){
		setZerosAndPeaks(signal, length, 100);
	}
	//std::list<int> zeroesAndPeaks = zerosAndPeaksGlobal;
	static std::list<int> slices = std::list<int>();
    int sign = zerosAndPeaks[0].peak > zerosAndPeaks[1].peak ? 1 : -1;
    int slicePoint = 0;
    int averagedChunk = 6;
    
    float average;
    std::vector<zeroAndPeak_t>::iterator i = zerosAndPeaks.begin();
    std::vector<zeroAndPeak_t>::iterator  lastZeroAndPeak = zerosAndPeaks.begin();
    /*for(i = zerosAndPeaks.begin(); i->zero != zerosAndPeaks[averagedChunk].zero && i != zerosAndPeaks.end(); i++){
        
    }*/
    while(i != zerosAndPeaks.end()){
        if(std::abs(i->peak - lastZeroAndPeak->peak) * sign < 0){
            if(sign == 1){
                slicePoint = lastZeroAndPeak->zero;
                sign *= -1;
            }
        }
        if(std::abs(i->peak - lastZeroAndPeak->peak) > threshold && i->zero - slices.back() > minSliceLen){
            slices.push_back(i->zero);
        }
        lastZeroAndPeak = i;
        i++;
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
	processAudioBuffer(buffer);
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
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
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

}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SwitcharooAudioProcessor();
}
