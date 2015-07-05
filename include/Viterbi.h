#ifndef VITERBI_H
#define VITERBI_H

#include "Chord.h"

class Viterbi
{
	public:
		Viterbi();
		virtual ~Viterbi();
		Chord viterbi(float *bassChroma, float *midChroma);
	protected:
	private:
		//pathProbs keeps track of the prior probability that the current chord is the chord with that index
		//stored as log of probability
		float pathProbs[NUM_CHORDS];

		float getTransProb(int prevChordId, int newChordId);
		bool normalize(float *arr,int arraySize);
		void getRootProbs(float *chroma, float *rootProbs);
		float getMidChordProb(float *chroma, Chord chord);
		void getChordProbs(float *bassChroma, float *midChroma, float *chordProbs);

};

#endif // VITERBI_H
