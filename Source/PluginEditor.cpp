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

//[Headers] You can add your own extra header files here...
//[/Headers]
#include "JuceHeader.h"
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <string>
//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SwitcharooAudioProcessorEditor::SwitcharooAudioProcessorEditor (SwitcharooAudioProcessor* ownerFilter)
    : AudioProcessorEditor(ownerFilter)
{
    addAndMakeVisible (slider = new Slider ("new slider"));
    slider->setRange (0, 10, 0);
    slider->setSliderStyle (Slider::LinearHorizontal);
    slider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    slider->addListener (this);

    addAndMakeVisible (textButton = new TextButton ("new button"));
    textButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
	getProcessor()->RequestUIUpdate();//UI update must be done each time a new editor is constructed
	startTimer(200);//starts timer with interval of 200mS
    //[/Constructor]
}

SwitcharooAudioProcessorEditor::~SwitcharooAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    slider = nullptr;
    textButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SwitcharooAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    g.setColour (Colour (0xff2aa568));
    g.fillRect (132, 204, 356, 164);

    g.setColour (Colour (0xff2a85a5));
    g.fillRect (156, 220, 308, 44);

    //[UserPaint] Add your own custom painting code here..
	if (thumbalina == NULL){
		g.setColour(Colour(0xffffdf85a5));
		//g.fillRect(0, 0, 200, 200);
		g.drawFittedText("so NULL text",
			10, 160, getWidth() - 20, getHeight() / 3,
			Justification::centred, 1);
		g.setFont(40);
	}
	else{
		g.setColour(Colour(0xffffdf85a5));
		//g.fillRect(50, 50, 100, 100);
		g.drawFittedText("not null text",
			10, 160, getWidth() - 20, getHeight() / 3,
			Justification::centred, 1);
		g.setFont(25);
	}
	
	if (thumbalina != NULL){
		Rectangle<int> rect(20, 0, getWidth()-40, getHeight() /2);
		g.setColour(Colour(0xcccccca5));
		g.fillRectList(rect);
		double len = thumbalina->getTotalLength();
		double start;
		double end;
		float zfact;
		start = 0;
		end = len;
		zfact = 1;
		thumbalina->drawChannels(g, rect, start, end, zfact);
		int num_samples = thumbalina->getNumSamplesFinished();
		String lenstr = "this is the length in seconds : " + std::to_string(len) + "samples :" + std::to_string(num_samples);
		g.setColour(Colour(0xdddddda5));
		g.drawFittedText(lenstr,
			10, 160, getWidth() - 20, getHeight() / 3,
			Justification::centred, 1);
	}
	
	//[/UserPaint]
}
/*draw time slice*/
const Line<float> SwitcharooAudioProcessorEditor::drawTimeSlice(Rectangle<int> & areaOfOutput, int numSamples, int samplePerPixel, int indexInSamples){
	//returns a line which you can draw.
	Line<float> f(1, 1, 1, 1);
	return f;
}

void SwitcharooAudioProcessorEditor::resized()
{
    slider->setBounds (241, 320, 150, 24);
    textButton->setBounds (233, 280, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SwitcharooAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == slider)
    {
        //[UserSliderCode_slider] -- add your slider handling code here..
        //[/UserSliderCode_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void SwitcharooAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]
	SwitcharooAudioProcessor * thisProcessor = new SwitcharooAudioProcessor();
    if (buttonThatWasClicked == textButton)
    {
        //[UserButtonCode_textButton] -- add your button handler code here..
		File compareFile = thisProcessor -> loadFile();
		//AudioSampleBuffer* buf = thisProcessor->processFile(compareFile);
		//setting up the audioThumbnail.
		AudioFormatManager* format = new AudioFormatManager();
		format->registerBasicFormats();
		setupThumb(format, compareFile);
		//[/UserButtonCode_textButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void SwitcharooAudioProcessorEditor::setupThumb(AudioFormatManager* format, File file){
	AudioThumbnailCache cache(5);
	AudioFormatReader * reader = format->createReaderFor(file);
	int numsamplesperThumb = reader->sampleRate;
	thumbalina = new AudioThumbnail(numsamplesperThumb, *format, cache);
	FileInputSource * thisFile = new FileInputSource(file);
	//InputSource * sourceFile = thisFile->createInputStream();
	thumbalina->setSource(thisFile);
	
	/* this will get the array of samples needed to slice*/
	/*
	AudioSampleBuffer * buf = new AudioSampleBuffer();
	reader->read(buf, 0, reader->lengthInSamples, 0, true, true);
	const float * array_of_samples= buf->getReadPointer(1);
	*/
	testGui = 40;
	repaint();
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void SwitcharooAudioProcessorEditor::timerCallback()

{

	SwitcharooAudioProcessor* ourProcessor = getProcessor();

	if(ourProcessor->NeedsUIUpdate())

	{//load your UI components with internal state information from plug-in - example:

		/*YourButtonName->setToggleState(1.0f == ourProcessor->getParameter(SwitcharooAudioProcessor::MasterBypass), false);*/

		//repeat for "OtherParams"...

		ourProcessor->ClearUIUpdateFlag();

	}

}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SwitcharooAudioProcessorEditor"
                 componentName="" parentClasses="public AudioProcessorEditor, public Timer"
                 constructorParams="SwitcharooAudioProcessor* ownerFilter" variableInitialisers="AudioProcessorEditor(ownerFilter)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff">
    <RECT pos="132 204 356 164" fill="solid: ff2aa568" hasStroke="0"/>
    <RECT pos="156 220 308 44" fill="solid: ff2a85a5" hasStroke="0"/>
  </BACKGROUND>
  <SLIDER name="new slider" id="a58cf13340e94247" memberName="slider" virtualName=""
          explicitFocusOrder="0" pos="241 320 150 24" min="0" max="10"
          int="0" style="LinearHorizontal" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <TEXTBUTTON name="new button" id="3b1e3b9b40732afd" memberName="textButton"
              virtualName="" explicitFocusOrder="0" pos="233 280 150 24" buttonText="new button"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
