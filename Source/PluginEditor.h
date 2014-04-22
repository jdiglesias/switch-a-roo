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

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);
	void setupThumb(AudioFormatManager* format, File compareFile);
	const Line<float> drawTimeSlice(Rectangle<int> & areaOfOutput, int numSamples, int indexInSamples);
	File SwitcharooAudioProcessorEditor::loadFile();
private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> slider;
    ScopedPointer<TextButton> textButton;
	AudioThumbnail * thumbalina = NULL;
	int64 totalNumSamples;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SwitcharooAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_67C500E2341A6342__
