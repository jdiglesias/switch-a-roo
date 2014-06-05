/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_67C500E2341A6342__
#define __JUCE_HEADER_67C500E2341A6342__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SwitcharooAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Timer,
                                        public SliderListener,
                                        public ButtonListener
{
public:
    //==============================================================================
    SwitcharooAudioProcessorEditor (SwitcharooAudioProcessor* ownerFilter);
    ~SwitcharooAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void timerCallback();

	SwitcharooAudioProcessor* getProcessor() const
	{
		return static_cast <SwitcharooAudioProcessor*>(getAudioProcessor());
	}
    //[/UserMethods]
	int fileNumba = 0;
	int widthOfRect;

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);

	void mouseDown(const MouseEvent &e){
		mousex = e.getMouseDownScreenX();
		mousey = e.getMouseDownScreenY();
		repaint();
		// do calculation then
		// do insert into timeslices
		// used in drawTimeslice
		int index = (totalNumSamples *(mousex-20 -190) / widthOfRect);
		tmp_index = index;
		if(!(index<0) && (fileNumba !=0)){
			if (fileNumba == 1){
				int real_index = thisProcessor->song1->nearestZero(index);
				if (!inSlice(real_index, 1)){
					timeSlices1->push_back(index);
					timeSlices1->sort();
				}
			}
			if (fileNumba == 2){
				int real_index = thisProcessor->song2->nearestZero(index);
				if (!inSlice(real_index, 2)){
					timeSlices1->push_back(index);
					timeSlices1->sort();
				}
			}
		}
		
		repaint();
	}
	bool inSlice(int index, int fileNum){
		if (fileNum == 1){
			for (std::list<int>::iterator iter = timeSlices1->begin(); iter != timeSlices1->end(); iter++){
				if (*iter == index)
					return true;
			}
		}
		if (fileNum == 2){
			for (std::list<int>::iterator iter = timeSlices1->begin(); iter != timeSlices2->end(); iter++){
				if (*iter == index)
					return true;
			}
		}
		return false;
	}

	void redoTimeSlices(float newThresh, int newMinSlices);

private:
	int tmp_index;
    //[UserVariables]   -- You can add your own custom variables in this section.
	std::vector<fftContainer> * transforms = NULL;
	fftContainer *retFFT = NULL;
	int fftLength;
	String global_reals;
	String global_img;
	int isSetTest = 0;
	float * fftRet = NULL;
	const Line<float> drawTimeSlice(Rectangle<int> & areaOfOutput, double seconds, int indexInSamples);
	AudioThumbnail * thumbalina = NULL;
	const float * arrayOsamps = NULL;

	File loadFile();
	void setupThumb(AudioFormatManager* format, File file, int fileNum);
	void writeComparison();
	std::list<int> * timeSlices1;
	std::list<int> * timeSlices2;
	AudioSampleBuffer * outSongBuf = NULL;
	float * outSongSamps = NULL;

	int64 totalNumSamples;
	double timeLen;
	double threshold;
	AudioSampleBuffer * curSongBuf = NULL;
	SwitcharooAudioProcessor * thisProcessor = new SwitcharooAudioProcessor();
	void registerFFT(File file);
	void doTestFFT(int N, float amplitude);
	int mousex, mousey;
	//[/UserVariables]
	// graphics debugs
	void fftSourceCompareDisplay(Graphics &g);
	void ThumbnailDisplay(Graphics &g);
	void doFFTdisplay(Graphics &g);
	void displayComparisonMsg(Graphics &g);
	void outputSongDisplay(Graphics &g);




    //==============================================================================
	ScopedPointer<Slider> sliderThresh;
	ScopedPointer<Slider> sliderMinlen;
    ScopedPointer<TextButton> selectSong1;
    ScopedPointer<TextButton> setSong1TimeSlices;
	ScopedPointer<TextButton> selectSong2;
	ScopedPointer<TextButton> setSong2TimeSlices;
	ScopedPointer<TextButton> fftOnSong;

	ScopedPointer<TextButton> doCompare;
	ScopedPointer<MouseListener> mouse;
		//MouseInputSourceInternal mouse;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SwitcharooAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_67C500E2341A6342__
