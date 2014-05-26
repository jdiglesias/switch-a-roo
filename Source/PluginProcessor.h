/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED
#include <list>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class SwitcharooAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SwitcharooAudioProcessor();
    ~SwitcharooAudioProcessor();


    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
       
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;
	

    //==============================================================================
    const String getName() const;

    int getNumParameters();
    const File loadFile();
    float getParameter (int index);
    void setParameter (int index, float newValue);
    
    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

	//added
	enum Parameters{ MasterBypass = 0, /*OtherParams...,*/ totalNumParam };

	bool NeedsUIUpdate(){ return UIUpdateFlag; };

	void RequestUIUpdate(){ UIUpdateFlag = true; };
	void ClearUIUpdateFlag(){ UIUpdateFlag = false; };
    const std::list<int>& getSlicesByAmplitude(const float signal[], const int length, const float threshold, int minSliceLen);

	AudioSampleBuffer * processFile(File compareFile);
	AudioSampleBuffer * readFile(File compareFile, AudioFormatManager* format);
	void processAudioBuffer(AudioSampleBuffer& buffer);
    typedef struct zeroAndPeak{
        int zero;
        float peak;
    } zeroAndPeak_t;
    
	AudioSampleBuffer testbuf;
	AudioThumbnail * inputAudioThumb = NULL;
private:
    std::vector<int> getSegments(float* channelData, int length);
    std::vector<zeroAndPeak_t> zerosAndPeaks = std::vector<zeroAndPeak_t>();
    void compareSamples(float* sourceData, float* sampleData, int sourceLength, int sampleLength, int offset);
    void setZerosAndPeaks(const float signal[], const int length, int averagedChunk);
	float UserParams[totalNumParam];
	bool UIUpdateFlag;
	
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SwitcharooAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
