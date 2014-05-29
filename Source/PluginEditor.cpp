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
#define _USE_MATH_DEFINES
#include <math.h>
#include "PluginEditor.h"
#include <string>

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]


//==============================================================================
SwitcharooAudioProcessorEditor::SwitcharooAudioProcessorEditor (SwitcharooAudioProcessor* ownerFilter)
    : AudioProcessorEditor(ownerFilter)
{
    addAndMakeVisible (sliderThresh = new Slider ("thresh"));
    sliderThresh->setRange (0, 1, 0);
	sliderThresh->setSliderStyle(Slider::LinearHorizontal);
	sliderThresh->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
	sliderThresh->addListener(this);

	addAndMakeVisible(sliderMinlen = new Slider("minLen"));
	sliderMinlen->setRange(0, 100000, 0);
	sliderMinlen->setSliderStyle(Slider::LinearHorizontal);
	sliderMinlen->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
	sliderMinlen->addListener(this);

    addAndMakeVisible (textButton = new TextButton ("new button"));
    textButton->addListener (this);

    addAndMakeVisible (dofft = new TextButton ("do fft"));
    dofft->setButtonText (TRANS("do fft button"));
    dofft->addListener (this);

	addAndMakeVisible(doCompare = new TextButton("do comparison"));
	doCompare->setButtonText(TRANS("do Comparison"));
	doCompare->addListener(this);

    setSize (900, 800);


    //[Constructor] You can add your own custom stuff here..
	getProcessor()->RequestUIUpdate();//UI update must be done each time a new editor is constructed
	startTimer(200);//starts timer with interval of 200mS
    //[/Constructor]
}

SwitcharooAudioProcessorEditor::~SwitcharooAudioProcessorEditor()
{
    sliderThresh = nullptr;
	sliderMinlen = nullptr;

    textButton = nullptr;
    dofft = nullptr;

}

//==============================================================================


void SwitcharooAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]
	String samplemsg = "";
	int slicesLen = 0;

    g.fillAll (Colours::white);
	g.drawSingleLineText("Thresh", 0, 750,Justification::left);
	g.drawSingleLineText("minSlice", 0, 770, Justification::left);
	g.drawMultiLineText(
		"mouse X:" + std::to_string(mousex) + " mouse Y:" + std::to_string(mousex) + 
		"tmp index" + std::to_string(tmp_index),
		getX(), 50, getWidth()
		);

	if (isSetTest == 1){
		g.drawMultiLineText(
			"imaginaries " + global_img + " reals " + global_reals,
			getX(), 20,
			getWidth());
	}
	if (!thisProcessor->fftVector.empty()){
		String msg = "amount of ffts ="+ std::to_string(thisProcessor->fftVector.size()) +"\n";
		for (int i = 0; i <= thisProcessor->fftVector.size()-1; i++){
			//get length
			int thisFFTlen = thisProcessor->fftVector[i].length;
			float thisFFTfund = thisProcessor->fftVector[i].fundamental;
			String ffts = "";
			for (int j = 0; j < 20; j++){
				ffts = ffts + std::to_string(thisProcessor->fftVector[i].amplitudes[j]) + ", ";
			}
			
			msg = msg + "FFT-" + std::to_string(i) + " length:" + std::to_string(thisFFTlen) + 
				" fundamental:" + std::to_string(thisFFTfund) +" first 20 ffts:" + ffts +"\n";
		}
		g.drawMultiLineText(msg,
			getX(), (getHeight() *2)/3,
			getWidth());

	}
	if (retFFT != NULL){
		String msg;
		int skipped = 0;
		msg = "";
		//prints a ratio of fundamentals
		int length = retFFT->length;
		float fund = retFFT->fundamental;
		float * retSignal = retFFT->amplitudes;
		if (length > 1000){
			length = 1000;
		} 
		for (int j = 0; j < 20; j++){
			if (!((int)retSignal[j] <= 0)){
				msg = msg + std::to_string(retSignal[j]/fund) + " ,";
			}
			else {
				msg = msg + " _ ";
				skipped++;
			}

			//msg = msg + "AUDIO" +std::to_string(retFFT->rawAudio[j]) + ",";
			//msg = msg + std::to_string(arrayOsamps[j]) + ", ";
		}
		
		g.drawMultiLineText("fftLength: " + std::to_string(retFFT->length) +" , skipped: "+std::to_string(skipped)+ " frequencies:" + msg,
			getX(), 20, //getHeight() /2,
			getWidth());
		
	}
	if (thumbalina != NULL){
		Rectangle<int> rect(20, 20, getWidth() - 40, getHeight() / 2);
		g.setColour(Colour(0xcc10b5ad));
		g.fillRectList(rect);
		double len = thumbalina->getTotalLength();
		double start;
		double end;
		float zfact;
		start = 0;
		end = len;
		zfact = 1;
		g.setColour(Colour(0xccFF0000));
		thumbalina->drawChannels(g, rect, start, end, zfact);
		//int num_samples = thumbalina->getNumSamplesFinished();

		g.setColour(Colour(0xff030303));

		/*trying to get audio from processBlock*/

		widthOfRect = rect.getWidth();
		timeLen = len;
		std::list<int>::iterator it;
		if (len != 0){
			for (std::list<int>::iterator it = timeSlices->begin(); it != timeSlices->end(); it++){
				samplemsg = samplemsg + std::to_string(*it) + ",";
				slicesLen = timeSlices->size();
				const Line<float> testline = drawTimeSlice(rect, len, *it);
				g.drawLine(testline);
			}

		}

	}
	g.drawMultiLineText("SliceSampleIndexes:" + samplemsg + " Number Of Slices:" + std::to_string(slicesLen), getX(), 30, getWidth());


}

/*draw time slice*/
const Line<float> SwitcharooAudioProcessorEditor::drawTimeSlice(Rectangle<int> & areaOfOutput, double seconds, int indexInSamples){
	//returns a line which you can draw.

	float right = areaOfOutput.getX();
	float topx, topy, bottomx, bottomy;
	//areaOfOutput.getWidth()/seconds;
	topx = (float)right + (indexInSamples / (totalNumSamples / seconds)) * (areaOfOutput.getWidth() / seconds) /*sample rate * index + offset*/;
	/*bottomx = (float) topx;
	*/
	//topy = (float) 0;

	bottomy = (float)areaOfOutput.getBottom();

	Line<float> f(topx, 20.0, topx, bottomy);

	return f;
}

void SwitcharooAudioProcessorEditor::resized()
{
    sliderThresh->setBounds (60, 730,800, 24);
	sliderMinlen->setBounds(60, 750, 800, 24);

    textButton->setBounds (750, 775, 150, 24);
    dofft->setBounds (400, 775, 150, 24);
	doCompare->setBounds(0, 775, 150, 24);

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SwitcharooAudioProcessorEditor::redoTimeSlices(int newThresh, int newMinSlices){
	timeSlices->clear();
	timeSlices = &thisProcessor->getSlicesByAmplitude(arrayOsamps, totalNumSamples, newThresh, newMinSlices);
	repaint();
} 


void SwitcharooAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]
	if (!timeSlices->empty()){
		if (sliderThatWasMoved == sliderThresh)
		{
			//[UserSliderCode_slider] -- add your slider handling code here..
			int newthresh = sliderThatWasMoved->getValue();
			thisProcessor->thresh = newthresh;
			redoTimeSlices(newthresh, thisProcessor->minSlices);
			//[/UserSliderCode_slider]
		}
		else if (sliderThatWasMoved = sliderMinlen){
			int newMinSlices = sliderThatWasMoved->getValue();
			thisProcessor->minSlices = newMinSlices;
			redoTimeSlices(thisProcessor->thresh, newMinSlices);

		}
	}
    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void SwitcharooAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == textButton)
    {
		File compareFile = loadFile();
		//setting up the audioThumbnail.
		if (compareFile != File()){
			AudioFormatManager* format = new AudioFormatManager();
			format->registerBasicFormats();
			setupThumb(format, compareFile);
		}
	}
    else if (buttonThatWasClicked == dofft)
    {
		File compFile = loadFile();
		if (compFile != File()){
			registerFFT(compFile);
			repaint();
		}
		/*
		doTestFFT(8, 1);
		repaint();
	*/
	}
	else if (buttonThatWasClicked == doCompare){
		File fileSource = loadFile();
		if (fileSource != File()){
			File fileCompare = loadFile();
			if (fileCompare != File()){
				thisProcessor ->doComparison(fileSource, fileCompare);
			}
		}
	}
	
    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

File SwitcharooAudioProcessorEditor::loadFile()
{
	FileChooser chooser("Please select the file you want to load...",
		File::getSpecialLocation(File::userHomeDirectory));
	if (chooser.browseForFileToOpen())
	{
		File file(chooser.getResult());
		return file;
	}
	return File();
}

void SwitcharooAudioProcessorEditor::registerFFT(File file){
	AudioFormatManager * format = new AudioFormatManager();
	format->registerBasicFormats();
	AudioFormatReader * reader = format->createReaderFor(file);
	AudioSampleBuffer * buffer = new AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
	
	int64 readerStartSample = 0;
	reader->read(buffer, 1, reader->lengthInSamples -1, readerStartSample +1, true, true);
	const float * samples = buffer->getReadPointer(0);
	int input = reader->lengthInSamples;
	fftContainer * ret = thisProcessor->doFFT(samples, reader->lengthInSamples);
	retFFT = ret;
}

void SwitcharooAudioProcessorEditor::doTestFFT(int N, float amplitude){
	kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 0, NULL, NULL);
	kiss_fft_scalar * fin = new kiss_fft_scalar[N];
	kiss_fft_cpx * fout = new kiss_fft_cpx[(N / 2) + 1];

	float * values = new float[8];
	values[0] = amplitude / 2;
	values[1] = amplitude;
	values[2] =  amplitude /2;
	values[3] = 0;
	values[4] = -amplitude / 2;
	values[5] = -amplitude;
	values[6] = -amplitude / 2;
	values[7] = 0;

	for (int i = 0; i < N; i++){
		fin[i] = values[i % 8];
	}

	String imagine;
	String reals;

	kiss_fftr(cfg, fin, fout);

	for (int j = 0; j < (N/2) +1; j++){
		imagine = imagine + std::to_string(fout[j].i) + ",";
		reals = reals + std::to_string(fout[j].r) + ",";
		//amplitude = std::to_string(fouttest[i]) + ",";
	}
	global_img = imagine;
	global_reals = reals;
	isSetTest = 1;
}

void SwitcharooAudioProcessorEditor::setupThumb(AudioFormatManager* format, File file){
	AudioThumbnailCache cache(5);
	AudioFormatReader * reader = format->createReaderFor(file);
	int numsamplesperThumb = reader->sampleRate;
	thumbalina = new AudioThumbnail(numsamplesperThumb, *format, cache);
	FileInputSource * thisFile = new FileInputSource(file);

	thumbalina->setSource(thisFile);

	/* this will get the array of samples needed to slice*/
	int64 readerStartSample = 0;
	AudioSampleBuffer * buf = new AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
	reader->read(buf, 1, reader->lengthInSamples-1, readerStartSample + 1, true, true);
	const float * array_of_samples = buf->getReadPointer(0);
	arrayOsamps = buf->getReadPointer(0);
	int64 tmpTotal = reader->lengthInSamples;
//to display audio
	timeSlices=	&thisProcessor->getSlicesByAmplitude(array_of_samples, reader->lengthInSamples, .005, 51000);
	totalNumSamples = tmpTotal;
//to slices to fft
//	thisProcessor->doFFTtoSlices(timeSlices, array_of_samples, reader->lengthInSamples);
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
  <TEXTBUTTON name="do fft" id="6647447847dc514e" memberName="dofft" virtualName=""
              explicitFocusOrder="0" pos="232 392 150 24" buttonText="new button"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
