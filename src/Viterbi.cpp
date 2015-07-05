#include <math.h>
#include "probtables.h"
#include "Viterbi.h"
#include <cstdio>

namespace chords {

Viterbi::Viterbi()
{
	for (int i=0;i<NUM_CHORDS;i++) {
		pathProbs[i]=1.0;
	}
	//ctor
}

Viterbi::~Viterbi()
{
	//dtor
}

/*
Returns probability of transitioning from one chord to another
After experimentation, this is dependent only on whether the chord is the same as the previous chord
*/
float Viterbi::getTransProb(int prevChordId, int newChordId) {
	//in order for probabilities to sum to one, if chords are the same, the probability of not transitioning is assigned
	//if chords are different, the probability of transitioning is divided among all different chords
	if (prevChordId==newChordId) return 0.25;
	else return 0.75/(NUM_CHORDS-1);
}

Chord Viterbi::viterbi(float *bassChroma, float *midChroma) {
	//paths=[-1,0,0,0]
	int prevChordIdx,chordIdx;
	float chordProbs[NUM_CHORDS];
	float prevChordProbs[NUM_CHORDS];
	float chordTransitionProb;
	float logTransProb;

	float prob;
	int maxPathProbIdx;
	float maxPathProb;
	int maxProbIdx;
	float maxProb;

	getChordProbs(bassChroma,midChroma,chordProbs);

	for (prevChordIdx=0;prevChordIdx<NUM_CHORDS;prevChordIdx++) prevChordProbs[prevChordIdx]=pathProbs[prevChordIdx];

	//update each chord probability value with the maximum possible value according to viterbi algorithm
	maxProb=0.0;
	maxProbIdx=-1;
	for (chordIdx=0;chordIdx<NUM_CHORDS;chordIdx++) {
		maxPathProbIdx=-1;
		maxPathProb=0.0;
		for (prevChordIdx=0;prevChordIdx<NUM_CHORDS;prevChordIdx++) {
			chordTransitionProb=getTransProb(prevChordIdx,chordIdx)*chordProbs[chordIdx];

			if (chordTransitionProb<=0.0) logTransProb=-100.0;
			else logTransProb=log(chordTransitionProb);
			prob=prevChordProbs[prevChordIdx]+logTransProb;

			if (prevChordIdx==0||prob>maxPathProb) {
				maxPathProb=prob;
				maxPathProbIdx=prevChordIdx;
			}
		}
		pathProbs[chordIdx]=maxPathProb;

		if (chordIdx==0||maxPathProb>maxProb) {
			maxProb=maxPathProb;
			maxProbIdx=chordIdx;
		}
	}

	Chord chord=maxProbIdx;
	return chord;
}

bool Viterbi::normalize(float *arr,int arraySize) {
	//normalizes array in-place so sum is 1.  Returns false if normalization fails
	//
	// If normalization fails (i.e. the sum of values is 0), values are all set to 1/arraySize

	int i;
	float total=0.0;

	for (i=0;i<arraySize;i++) total+=arr[i];
	if (total==0) {
		float val=1.0/arraySize;
		for (i=0;i<arraySize;i++) arr[i]=val;
		return false;
	}
	else {
		for (i=0;i<arraySize;i++) arr[i]/=total;
		return true;
	}
}

// ------------- bass chroma -----------------
// Given bass chroma, finds probability that each note is the root and places it in rootProbs array
// rootProbs and chroma are arrays of size 12, each value representing one note of chromagram
void Viterbi::getRootProbs(float *chroma, float *rootProbs) {

	int i;
	bool success;

	int normalizedChromaVals[12];
	int chromaRootVal;
	int chromaFifthVal;

	success=normalize(chroma,12);
	if (!success) {
		//chroma is empty, return equal probabilities
		for (i=0;i<12;i++) rootProbs[i]=1.0/12.0;
		return;
	}

	//convert normalized chroma from 0...1 real scale to integer scale from 0 to 10
	//these values are the indexes of the lookup table for bass probabilities
	for (i=0;i<12;i++) {
		normalizedChromaVals[i]=(int)ceil(10*chroma[i]);
	}
	for (i=0;i<12;i++) {
		chromaRootVal=normalizedChromaVals[i];
		chromaFifthVal=normalizedChromaVals[(i+7)%12];
		//get the probability from a lookup table given the chroma values of the root and fifth
		//where i is the root index
		rootProbs[i]=bassProbTable[chromaRootVal][chromaFifthVal];
	}
	normalize(rootProbs,12);
}

// ------------------ mid chroma ----------------------
float Viterbi::getMidChordProb(float *chroma, Chord chord) {
	//finds probability of chord with root note chordRootId and type chordTypeId
	//by finding the product pdf values of each of the 12 pitches for that chord
	//pdf is sum of 2 Gaussian curves.  Parameters for pdf are stored in a lookup table
	//These parameters were learned from machine learning.  There is another lookup table
	//for empty chroma bins (whose value approaches zero)

	int chordTypeId=chord.getChordTypeId();
	int chordRootId=chord.getChordRootId();

	float prob=1.0;
	int interval; //in semis, i.e. 0=root 1==minor second, 2=major second, 3=minor third, 4=major third ... up to 11
	int noteId;
	float x,y,mu,sigma,d,a1,a2;

	for (interval=0;interval<12;interval++) {
		noteId=(chordRootId+interval)%12;
		x=chroma[noteId];
		if (x<0.01) y=chordZeroTable[chordTypeId][interval];
		else {
			//get parameters (amplitude, mean, standard dev) - a2 is for 2nd gaussian with fixed deviation
			a1=chordGaussTable[chordTypeId][interval][0];
			mu=chordGaussTable[chordTypeId][interval][1];
			sigma=chordGaussTable[chordTypeId][interval][2];
			a2=chordGaussTable[chordTypeId][interval][3];

			d=(x-mu)/sigma;
			//sum of 2 gaussians
			y=a1*exp(-0.5*d*d)+a2*exp(-7*fabs(x-mu));
		}
		prob*=y;
	}
	return prob;
}

void Viterbi::getChordProbs(float *bassChroma, float *midChroma, float *chordProbs) {
	int i;
	float rootProbs[12];

	normalize(midChroma,12);
	getRootProbs(bassChroma,rootProbs);

	Chord chord;
	for (i=0;i<NUM_CHORDS;i++) {
		chord=i;
		//probability of chord given bass note probability, chord type probability, and treble notes chord probability
		chordProbs[i]=rootProbs[chord.getChordRootId()]*getMidChordProb(midChroma,chord)*chordTypeProbs[chord.getChordTypeId()];
	}

	normalize(chordProbs,60);
}

}
