#include "ChordDetect.h"
#include "ChordDetectUI.h"
#include <String>
#include <cmath>
#include <sstream>
#include <fstream>
#include "fft.h"
#include "PeakFinder.h"
#include "Chord.h"

//std::ofstream logger;

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ChordDetect (audioMaster);
}

ChordDetect::ChordDetect(audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster,1,0)
{
	bufferSize=4096;
	hopSize=1024;
	inputSampleRate=44100.0;
	fftSampleRate=11025.0;
	sampleIncrement=4;

	//ctor
	setNumInputs(1);
	setNumOutputs(1);
	canProcessReplacing(true);

	editor=new ChordDetectUI(this);
	setUniqueID('T6E3');
	//((ChordDetectUI *)editor)->updateChord("tst2");
	buffer=new float[bufferSize];
	bufferReadIdx=0;
	bufferWriteIdx=0;
	fft=new FFT(bufferSize);
	//logger.open("C:\\apps\\vstlog.txt");
	//logger << "Line 3\n";

	resume();
}

ChordDetect::~ChordDetect()
{
	//dtor
	//logger.close();
}

bool ChordDetect::getEffectName (char* name)
{
	strcpy (name, "ChordDetect");
	return true;
}

//------------------------------------------------------------------------
bool ChordDetect::getProductString (char* text)
{
	strcpy (text, "ChordDetect");
	return true;
}

//------------------------------------------------------------------------
bool ChordDetect::getVendorString (char* text)
{
	strcpy (text, "Frozen Caveman Media");
	return true;
}

void ChordDetect::resume() {
}

void ChordDetect::setSampleRate (float sampleRate) {
	//logger << "Sample rate = ";
	//logger << sampleRate;
	//logger << "\n";
	inputSampleRate=sampleRate;
	sampleIncrement=(int)ceil(sampleRate/11025);
	fftSampleRate=sampleRate/sampleIncrement;
}
//---------------------------------------------------------------------------

void ChordDetect::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames) {
	outputs=inputs;

	int i;
	bool isUpdated=false;
	float bassChroma[12];
	float midChroma[12];
	std::vector<float> spec;
	Chord chord;

	for (i=0;i<sampleFrames;i+=sampleIncrement) {
		buffer[bufferWriteIdx]=inputs[0][i];
		bufferWriteIdx++;
		bufferWriteIdx%=bufferSize;

		if (bufferWriteIdx==bufferReadIdx) {
			spec=fft->windowed_fft(buffer,bufferReadIdx,bufferSize);
			peakFinderModule.getChromas(bassChroma,midChroma,spec,fftSampleRate);
			chord=viterbiModule.viterbi(bassChroma,midChroma);
			((ChordDetectUI *)editor)->updateChord(chord.getChordName().c_str());
			isUpdated=true;
			bufferReadIdx+=hopSize;
			bufferReadIdx%=bufferSize;
		}

	}

	//logger << "process replacing - \n";
}
