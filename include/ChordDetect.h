#ifndef CHORDDETECT_H
#define CHORDDETECT_H

#include "audioeffectx.h"
#include "fft.h"
#include "PeakFinder.h"
#include "Viterbi.h"
#include <fstream>

const char chordNames[5][4]={"","m","dim","aug","sus"};
const char noteNames[12][3]={"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};


class ChordDetect : public AudioEffectX {
	public:
		ChordDetect (audioMasterCallback audioMaster);
		~ChordDetect();
		virtual bool getEffectName (char* name);
		virtual bool getVendorString (char* text);
		virtual bool getProductString (char* text);
		virtual VstInt32 getVendorVersion () { return 1000; }
		virtual void resume();
		virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
		virtual void setSampleRate (float sampleRate);
	protected:
	private:
		float *buffer;
		int bufferSize;
		int hopSize;
		int bufferReadIdx;
		int bufferWriteIdx;
		float inputSampleRate;
		int sampleIncrement;
		float fftSampleRate;
		FFT *fft;
		PeakFinder peakFinderModule;
		Viterbi viterbiModule;
};

#endif // CHORDDETECT_CPP_H
