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

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
//    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
  //  {
    //    buffer.clear (i, 0, buffer.getNumSamples());
    //}
}
     


        
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


fftContainer * SwitcharooAudioProcessor::doFFT(const float * signal, int signalLength){
	int nextPow2 = 1;
	//read in musi-math need for optimization
	while (nextPow2 < signalLength) {
		nextPow2 *= 2;
	}

	kiss_fftr_cfg cfg = kiss_fftr_alloc(nextPow2, 0, NULL, NULL);
	kiss_fft_scalar * fin = new kiss_fft_scalar[nextPow2];
	kiss_fft_cpx * fout = new kiss_fft_cpx[(nextPow2 / 2) + 1];

	/*ignore first sample might be header data*/
	fin[0] = 0;
	for (int j = 1; j < signalLength; j++){
		fin[j] = signal[j];
	}
	fin[signalLength + 1] = 0;
	//pad out next part to make the signal a power of two.
	int temp_input = signalLength + 1;
	while (temp_input < nextPow2){
		fin[temp_input] = 0;
		temp_input++;
	}
	
	float * fftSignal = new float[(nextPow2 / 2) + 1];
	
	std::complex<float> * real_signal = new std::complex<float>[(nextPow2 / 2) + 1];
	kiss_fftr(cfg, fin, fout);
	float fund = 0.0;
	float cur_real, cur_img;
	float real = 0;
	for (int k = 0; k < (nextPow2 / 2) + 1; k++){
		cur_real = fout[k].r / (nextPow2 / 2);
		cur_img = fout[k].i / (nextPow2 / 2);

		real = (cur_real * cur_real) + (cur_img * cur_img);
		fftSignal[k] = sqrt(real)/44100;
		if (fund < sqrt(real)){
			fund = sqrt(real);
		}
		real_signal[k] = std::complex<float>(cur_real, cur_img);
	}
	float * rawAudio =new float[signalLength];
	for (int a = 0; a < signalLength; a++){
		rawAudio[a] = signal[a];
	}
	 //fftContainer retFFT((nextPow2 / 2) + 1, fund, fftSignal, real_signal);
	fftContainer * retFFT = new fftContainer((nextPow2 / 2) + 1, fund, fftSignal, real_signal, rawAudio, signalLength);
	free(cfg);
	return retFFT;
}

void SwitcharooAudioProcessor::doFFTtoSlices(std::list<int> timeslices, const float signal[], int signalLength, String which){
	//for time slices
	if (which == "source"){
		fftSource = std::vector<fftContainer>();
	}
	if (which == "compare"){
		fftCompare = std::vector<fftContainer>();
	}

	fftContainer * cur_slice;
	const float * sig;
	int time_slice_loop = 0;
	int prev_slice;
	int last_slice =0;
	int cur_slice_len;
	float * sigStart;
	for (std::list<int>::iterator it = timeslices.begin(); it != timeslices.end(); it++){
		//skip 0 slice
		if (*it != 0){
	
			//sets length of slice
			if (time_slice_loop == 0){
				prev_slice = 0;
			}
			else{
				it--;
				prev_slice = *it;
				it++;
			}
			cur_slice_len = *it - prev_slice;
			sigStart = new float[cur_slice_len];
			int i = prev_slice;
			int index = 0;
			for (i; i < *it; i++){
				sigStart[index] = signal[i];
				index++;
			}

			cur_slice = doFFT(sigStart, cur_slice_len);
			//dereferencing should be ok.
			if (which == "source"){
				fftSource.push_back(*cur_slice);
			}
			if (which == "compare"){
				fftCompare.push_back(*cur_slice);
			}
			time_slice_loop++;
		}
		last_slice = *it;
	}

	sigStart = new float[signalLength - last_slice];
	int j = last_slice;
	int index = 0;
	for (j; j < signalLength; j++){
		sigStart[index] = signal[j];
		index++;
	}
	cur_slice_len = signalLength - last_slice;
	cur_slice = doFFT(sigStart, cur_slice_len);
	if (which == "source"){
		fftSource.push_back(*cur_slice);
	}
	if (which == "compare"){
		fftCompare.push_back(*cur_slice);
	}

	//fftVector.push_back(retFFT);

}


/*
comparison stuff
      -------------------------------------------------------
source|  1             |        2        |       3          |
	  -------------------------------------------------------
        |     \    __\____/____|_______\__/ /     |
        X      X   X  O       skip      X skip    O
        |       \  |   \       |         \/       |
       -------------------------------------------------------
compare|  1     |  2     |       3        |        4         |
       -------------------------------------------------------
this is what basic comparison looks like S1 compares with  C1 and C2 before succeeding on C3. 
it should then immediately incremement what you want to compare so S2 will look at C1 ,C2 ,skip C3, and C4. we thus leave S2 alone
S3 then looks at C1, C2, skip c3, and then matches on c4.
the values are pushed back on to a list of inorder length and floats
the values are then written with a audioFormatWriter.
*/

void SwitcharooAudioProcessor::doComparison(songProperties * source, songProperties * compare){
	doFFTtoSlices(*source->timeSlices, source->samples, source->totalNumSamples, "source");
	doFFTtoSlices(*compare->timeSlices, compare->samples, compare->totalNumSamples, "compare");
	int totalLen = 0;
	float * rawAudioCopy;
	std::vector< std::pair<float *, int>> matched;
	for (int i = 0; i < fftSource.size() - 1; i++){
		float * sampStart;
		int thisSliceLen =0;
		for (int j = i; j < fftCompare.size() - 1; j++){
			//here is where the matching happens.  
			bool isMatch = fftMatch(fftSource[i], fftCompare[j]);
			//matching function
			if (isMatch){
				//call match for now.
				sampStart = new float[fftCompare[j].rawAudioLength];
				for (int a = 0; a < fftCompare[j].rawAudioLength; a++){
					sampStart[a] = fftCompare[j].rawAudio[a];
				}
				thisSliceLen = fftCompare[j].rawAudioLength;

				
				matched.push_back(std::pair<float *, int>(sampStart, thisSliceLen));
				totalLen += fftCompare[j].rawAudioLength;
				//want to look at the next slice to compare.
				if (i < fftSource.size() - 1){
					i++;
					//go back to first sample
					//j=0
				}
				if (i >= fftSource.size() - 1){
					//escape to upper for loop or it should be done i guess.
					break;
				}

			}
			else{
				thisSliceLen = 0;
			}
		}
		sampStart = new float[fftSource[i].rawAudioLength];
		for (int a = 0; a < fftSource[i].rawAudioLength; a++){
			sampStart[a] = fftSource[i].rawAudio[a];
		}
		thisSliceLen = fftSource[i].rawAudioLength;
		matched.push_back(std::pair<float *, int>(sampStart, thisSliceLen));
		totalLen += fftSource[i].rawAudioLength;
	}
	outputSong = matched;
	outSongLength = totalLen;
}

bool SwitcharooAudioProcessor::fftMatch(fftContainer source, fftContainer compare){
	//fft lengths should be the same.
	if (source.length != compare.length){
		return false;
	}
	int match = 0;
	for (int i = 0; i < source.length; i++){
		//greater by .1 or less by .1
		if (source.amplitudes[i] >= (compare.amplitudes[i] - .1) && source.amplitudes[i] <= (compare.amplitudes[i] + .1)){
			match++;
		}
	}
	if (match > source.length * 3 / 4){
		return true;
	}
	else{
		false;
	}

}
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SwitcharooAudioProcessor();
}
