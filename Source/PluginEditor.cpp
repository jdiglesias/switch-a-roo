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

    addAndMakeVisible (selectSong1 = new TextButton ("Select Song 1"));
	selectSong1->setButtonText(TRANS("Select first Song"));
	selectSong1->addListener(this);

    addAndMakeVisible (setSong1TimeSlices = new TextButton ("do fft"));
	setSong1TimeSlices->setButtonText(TRANS("set Song 1 slices"));
	setSong1TimeSlices->addListener(this);

	addAndMakeVisible(selectSong2 = new TextButton("Select Song 1"));
	selectSong2->setButtonText(TRANS("Select second Song"));
	selectSong2->addListener(this);

	addAndMakeVisible(setSong2TimeSlices = new TextButton("do fft"));
	setSong2TimeSlices->setButtonText(TRANS("set Song 2 slices"));
	setSong2TimeSlices->addListener(this);

	addAndMakeVisible(doCompare = new TextButton("do comparison"));
	doCompare->setButtonText(TRANS("do Comparison"));
	doCompare->addListener(this);
	addAndMakeVisible(fftOnSong = new TextButton("do comparison"));
	fftOnSong->setButtonText(TRANS("do FFT song"));
	fftOnSong->addListener(this);


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

	selectSong1 = nullptr;
	selectSong2 = nullptr;

	setSong1TimeSlices = nullptr;
	setSong2TimeSlices = nullptr;

}

//==============================================================================


void SwitcharooAudioProcessorEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]


    g.fillAll (Colours::white);
	g.drawSingleLineText("Thresh", 0, 750,Justification::left);
	g.drawSingleLineText("minSlice", 0, 770, Justification::left);
	g.drawMultiLineText(
		"mouse X:" + std::to_string(mousex) + " mouse Y:" + std::to_string(mousex) + 
		"tmp index" + std::to_string(tmp_index) + " File Numba: " + std::to_string(fileNumba),
		getX(), getHeight() /2 +30, getWidth()
	);
	if (outSongBuf != NULL){
		String outSamps = "";
		for (int a = 0; a < 30; a++){
			outSamps = outSamps + std::to_string(outSongSamps[a]) + ",";
		}
		String outSampLens = "";
		for (int b = 0; b < thisProcessor->outputSong.size() - 1; b++){
			outSampLens = outSampLens + std::to_string(thisProcessor->outputSong[b].second) + ",";
		}
		g.drawMultiLineText(
			"Out Song buffers length" + std::to_string(outSongBuf->getNumSamples()) +
			"Out song samps:" + outSamps +
			"slice lengths:" + outSampLens,
			getX(), getHeight() / 2 + 60, getWidth()
			);

	}
	//outputSongDisplay(g);
	//displayComparisonMsg(g);
	//fftSourceCompareDisplay(g);
	doFFTdisplay(g);
	ThumbnailDisplay(g);


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

	selectSong1->setBounds(750, 775, 125, 24);
	setSong1TimeSlices->setBounds(625, 775, 125, 24);

	selectSong2->setBounds(500, 775, 125, 24);
	setSong2TimeSlices->setBounds(375, 775, 125, 24);
	fftOnSong->setBounds(225, 775, 125, 24);

	doCompare->setBounds(0, 775, 150, 24);

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SwitcharooAudioProcessorEditor::redoTimeSlices(float newThresh, int newMinSlices){
	if (fileNumba == 1){
		timeSlices1->clear();
		timeSlices1 = &thisProcessor->song1->getSlicesByAmplitude(arrayOsamps, totalNumSamples, newThresh, newMinSlices);
	}
	if (fileNumba == 2) {
		timeSlices2->clear();
		timeSlices2 = &thisProcessor->song2->getSlicesByAmplitude(arrayOsamps, totalNumSamples, newThresh, newMinSlices);
	}
	repaint();
} 


void SwitcharooAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]
	bool check;
	if (fileNumba == 1){
		check = !timeSlices1->empty();
	}
	if (fileNumba == 2){
		check = !timeSlices2->empty();
	}
	if (check){
		if (sliderThatWasMoved == sliderThresh)
		{
			//[UserSliderCode_slider] -- add your slider handling code here..
			float newthresh = sliderThatWasMoved->getValue();
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

    if (buttonThatWasClicked == selectSong1)
    {
		File compareFile = loadFile();
		//setting up the audioThumbnail.
		if (compareFile != File()){
			AudioFormatManager* format = new AudioFormatManager();
			format->registerBasicFormats();
			setupThumb(format, compareFile, 1);
		}
	}
	else if (buttonThatWasClicked == setSong1TimeSlices)
	{
		//set the processors firt song;
		thisProcessor->song1->setTimeSlices(*timeSlices1);
		fileNumba = 12;
		timeSlices1->clear();
		repaint();
		setSong1TimeSlices->setButtonText(TRANS("SONG 1 SET!"));


	}
	else if (buttonThatWasClicked == selectSong2)
	{
		File compareFile = loadFile();
		//setting up the audioThumbnail.
		if (compareFile != File()){
			AudioFormatManager* format = new AudioFormatManager();
			format->registerBasicFormats();
			setupThumb(format, compareFile, 2);
		}
	}
	else if (buttonThatWasClicked == setSong2TimeSlices)
	{
		//set the processors firt song;
		thisProcessor->song2->setTimeSlices(*timeSlices2);
		fileNumba = 12;
		repaint();
		setSong2TimeSlices->setButtonText(TRANS("SONG 2 SET!"));
	}
    else if (buttonThatWasClicked == fftOnSong)
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
		thisProcessor->doComparison(thisProcessor->song1, thisProcessor->song2);
		//then write
		doCompare->setButtonText(TRANS("ALMOST THERE")); 
		writeComparison();
		doCompare->setButtonText(TRANS("Wrote Something"));
		repaint();
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

void SwitcharooAudioProcessorEditor::setupThumb(AudioFormatManager* format, File file, int fileNum){
	fileNumba = fileNum;
	AudioThumbnailCache cache(5);
	AudioFormatReader * reader = format->createReaderFor(file);
	int numsamplesperThumb = reader->sampleRate;
	thumbalina = new AudioThumbnail(numsamplesperThumb, *format, cache);
	FileInputSource * thisFile = new FileInputSource(file);

	thumbalina->setSource(thisFile);
	/* this will get the array of samples needed to slice*/
	int64 readerStartSample = 0;
	AudioSampleBuffer * curBuf = new AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
	curBuf->clear();
	reader->read(curBuf, 1, reader->lengthInSamples-1, readerStartSample + 1, true, true);
	const float * array_of_samples = curBuf->getReadPointer(1);
	float * tmp_array = new float[reader->lengthInSamples];
	for (int i = 0; i < curBuf->getNumSamples(); i++){
		tmp_array[i] = array_of_samples[i];
	}
	arrayOsamps = tmp_array;
//to display audio
	//songProperties tmp_song);


	if (fileNum == 1){
		thisProcessor->song1 = new songProperties(reader->sampleRate, reader->lengthInSamples,
			thumbalina->getTotalLength(), arrayOsamps, .5, 2000);
		timeSlices1 = &thisProcessor->song1->getSlicesByFrequency(array_of_samples, reader->lengthInSamples, 300, 1, 3000);
		//timeSlices1 = &thisProcessor->song1->getSlicesByAmplitude(arrayOsamps, reader->lengthInSamples, .005, 2000);
	}
	if (fileNum == 2){
		thisProcessor->song2 = new songProperties(reader->sampleRate, reader->lengthInSamples,
			thumbalina->getTotalLength(), arrayOsamps, .5, 2000);
		timeSlices2 = &thisProcessor->song2->getSlicesByFrequency(array_of_samples, reader->lengthInSamples, 300, 1, 3000);

		//timeSlices2 = &thisProcessor->song2->getSlicesByAmplitude(arrayOsamps, reader->lengthInSamples, .005, 2000);
	}
	totalNumSamples = reader->lengthInSamples;
	repaint();
}

void SwitcharooAudioProcessorEditor::writeComparison(){
	File outSong = File(File::getSpecialLocation(File::userHomeDirectory).getChildFile("outSong.wav"));
	TemporaryFile out(outSong);
	if (outSong.existsAsFile()){
		outSong.deleteFile();
	}	
	WavAudioFormat wavFormat;
	int len = thisProcessor->outSongLength;

	ScopedPointer <OutputStream> outStream(outSong.createOutputStream());
	//FileOutputStream * outSongStream = outSong.createOutputStream(len);
	//FileOutputStream outSongStream(outSong);
	float * samps = new float[len];
	int index = 0;

	if (!thisProcessor->outputSong.empty()){
		//we are ok
		for (int cur_clip = 0; cur_clip < thisProcessor->outputSong.size() - 1; cur_clip++){
			int curSampLen = thisProcessor->outputSong[cur_clip].second;
			for (int a = 1; a < curSampLen; a++){
					samps[index] = thisProcessor->outputSong[cur_clip].first[a];
					index++;
				}
			}
	}


	/*
	TESTING SHIT
	float * samps = new float[40000];
	for (int i = 0; i < 40000; i++){
		samps[i] = .5666;
	}

	AudioFormatManager * format = new AudioFormatManager();
	format->registerBasicFormats();
	AudioFormatReader * reader = format->createReaderFor(testSong);
	AudioSampleBuffer * buf = new AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
	reader->read(buf, 1, reader->lengthInSamples - 1, 1, true, true);
	*/

	//const float * newSamps = samps;
	AudioSampleBuffer buf(1, len);
	buf.copyFrom(0, 0, samps, len);
	Array<int> posBits = wavFormat.getPossibleBitDepths();
	StringPairArray metaData = wavFormat.createBWAVMetadata("wombo combo", "switcharoo",
		"switchdedooo", Time::getCurrentTime(), 44100, "2013-2014");
	ScopedPointer <AudioFormatWriter> writer = wavFormat.createWriterFor(outStream, 44100.0, buf.getNumChannels(),
		32, metaData, 0);
	outStream.release();

	writer->writeFromAudioSampleBuffer(buf, 0, buf.getNumSamples());
		//delete (&outSongStream);
		out.overwriteTargetFileWithTemporary();
		writer = nullptr;
		outSongBuf = &buf;
		outSongSamps = samps;
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
/*********************************/
//    Debug DISPLAY FUNCTIONS    //
/*********************************/
void SwitcharooAudioProcessorEditor::fftSourceCompareDisplay(Graphics &g){
	if (!thisProcessor->fftCompare.empty()){
		String msg = "amount of ffts =" + std::to_string(thisProcessor->fftCompare.size()) + "\n";
		for (int i = 0; i <= thisProcessor->fftCompare.size() - 1; i++){
			//get length
			int thisFFTlen = thisProcessor->fftCompare[i].rawAudioLength;
			float thisFFTfund = thisProcessor->fftCompare[i].fundamental;
			String ffts = "";
			for (int j = 0; j < 20; j++){
				ffts = ffts + std::to_string(thisProcessor->fftCompare[i].rawAudio[j]) + ", ";
			}

			msg = msg + "FFT-" + std::to_string(i) + " length:" + std::to_string(thisFFTlen) +
				" fundamental:" + std::to_string(thisFFTfund) + " first 20 ffts:" + ffts + "\n";
		}
		g.drawMultiLineText(msg,
			getX(), (getHeight()) / 3,
			getWidth());

	}

	if (!thisProcessor->fftSource.empty()){
		String msg = "amount of ffts =" + std::to_string(thisProcessor->fftSource.size()) + "\n";
		for (int i = 0; i <= thisProcessor->fftSource.size() - 1; i++){
			//get length
			int thisFFTlen = thisProcessor->fftSource[i].rawAudioLength;
			float thisFFTfund = thisProcessor->fftSource[i].fundamental;
			String ffts = "";
			for (int j = 0; j < 20; j++){
				ffts = ffts + std::to_string(thisProcessor->fftSource[i].rawAudio[j]) + ", ";
			}

			msg = msg + "FFT-" + std::to_string(i) + " length:" + std::to_string(thisFFTlen) +
				" fundamental:" + std::to_string(thisFFTfund) + " first 20 ffts:" + ffts + "\n";
		}
		g.drawMultiLineText(msg,
			getX(), (getHeight() * 2) / 3,
			getWidth());

	}
}

void SwitcharooAudioProcessorEditor::ThumbnailDisplay(Graphics &g){
	String samplemsg = "";
	int slicesLen = 0;
	if (thumbalina != NULL && (fileNumba == 1 || fileNumba == 2)){
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
			if (fileNumba == 1){
				for (it = timeSlices1->begin(); it != timeSlices1->end(); it++){
					samplemsg = samplemsg + std::to_string(*it) + ",";
					slicesLen = timeSlices1->size();
					const Line<float> testline = drawTimeSlice(rect, len, *it);
					g.drawLine(testline);
				}
				samplemsg = samplemsg + "--------doing slices 1-------";
			}
			if (fileNumba == 2) {
				for (it = timeSlices2->begin(); it != timeSlices2->end(); it++){
					samplemsg = samplemsg + std::to_string(*it) + ",";
					slicesLen = timeSlices2->size();
					const Line<float> testline = drawTimeSlice(rect, len, *it);
					g.drawLine(testline);
				}
				samplemsg = samplemsg + "----------doing slices 2 -------";

			}

		}

	}
	g.drawMultiLineText("SliceSampleIndexes:" + samplemsg + " Number Of Slices:" + std::to_string(slicesLen),
		getX(), getHeight() / 2 + 45, getWidth());

}
void SwitcharooAudioProcessorEditor::doFFTdisplay(Graphics &g){
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
			if (!(retSignal[j] < 0)){
				msg = msg + std::to_string(retSignal[j]) + " ,";
			}
			else {
				msg = msg + " _ ";
				skipped++;
			}

			//msg = msg + "AUDIO" +std::to_string(retFFT->rawAudio[j]) + ",";
			//msg = msg + std::to_string(arrayOsamps[j]) + ", ";
		}

		g.drawMultiLineText("fftLength: " + std::to_string(retFFT->length) + " , skipped: " + std::to_string(skipped) + " frequencies:" + msg,
			getX(), 20, //getHeight() /2,
			getWidth());

	}
}
void SwitcharooAudioProcessorEditor::displayComparisonMsg(Graphics &g){
	if (thisProcessor->song1 != NULL && thisProcessor->song2 != NULL){
		int len1 = thisProcessor->song1->timeSlices->size();
		int len2 = thisProcessor->song2->timeSlices->size();
		String songSamps = "";
		for (int i = 0; i < 10; i++){
			songSamps = songSamps + std::to_string(thisProcessor->song1->samples[i]) + ",";
			songSamps = songSamps + std::to_string(thisProcessor->song2->samples[i]) + ",";
		}
		String totalNums = "";
		totalNums = " Song1_totalSamps:" + std::to_string(thisProcessor->song1->totalNumSamples)
		+ " Song2_totalSamps:" + std::to_string(thisProcessor->song2->totalNumSamples);
		g.drawMultiLineText(
		"YOU CAN DO A COMPARISON!!! slices in song 1:" + std::to_string(len1)
		+ " amount in song 2:" + std::to_string(len2)
		+ " songSamps" + songSamps + totalNums,
		getX(), getHeight() / 2 + 200, getWidth());
	}

}

void SwitcharooAudioProcessorEditor::outputSongDisplay(Graphics &g){
	if (!thisProcessor->outputSong.empty()){
		String combined = "A bunch of Slices and their audio samples \n";
		for (int i = 0; i < thisProcessor->outputSong.size() - 1; i++){
			String sliceLen = std::to_string(thisProcessor->outputSong[i].second);
			String sliceAudio = "start of slice--" + std::to_string(i) + "\'s audio";
			for (int b = 0; b < 20; b++){
				sliceAudio = sliceAudio + std::to_string(thisProcessor->outputSong[i].first[b])+",";
			}
			combined = combined + sliceLen + sliceAudio + "\n";
		}
		g.drawMultiLineText(
			combined,
			getX(), getHeight() / 2 + 200, getWidth());
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
