#include <math.h>
#include "probtables.h"
#include "Viterbi.h"
#include <cstdio>


Viterbi::Viterbi()
{
	int i;
	for (i=0;i<60;i++) {
		pathProbs[i]=1.0;
		pathChordLengths[i][0]=0;
		pathChordLengths[i][1]=0;
	}
	//ctor
}

Viterbi::~Viterbi()
{
	//dtor
}

float Viterbi::getTransProb(int chordLen, int prevChordLen) {
	int chordLenDiff;
	float prob;

	chordLenDiff=fabs(chordLen-prevChordLen);
	if (chordLen==0&&prevChordLen==0) return 1.0;

	//if (chordLenDiff==0) prob=0.7;
	//else if (chordLenDiff==1) prob=0.5;
	//else prob=0.15;
	prob=0.3;

	if (chordLen<8) prob*=chordLen/8.0;

	return 0.75;
}

int Viterbi::viterbi(float *bassChroma, float *midChroma) {
	//paths=[-1,0,0,0]
	int pathIdx,chordIdx;
	float chordProbs[60];
	float prevPathProbs[60];
	float transMtx[60][60];
	float transProb;
	float chordTransProb;

	float prob;
	int maxPathProbIdx;
	float maxPathProb;
	int maxProbIdx;
	float maxProb;

	getChordProbs(bassChroma,midChroma,chordProbs);

	//adjust chord probabilities for each path according to transition probabilities
	//i.e. more likely if chord is same as previous chord
	for (pathIdx=0;pathIdx<60;pathIdx++) {
		transProb=getTransProb(pathChordLengths[pathIdx][0],pathChordLengths[pathIdx][1]);
		for (chordIdx=0;chordIdx<60;chordIdx++) {
			if (pathIdx==chordIdx) chordTransProb=1.0-transProb;
			else chordTransProb=transProb/59.0;
			transMtx[pathIdx][chordIdx]=chordTransProb*chordProbs[chordIdx];
		}
		normalize(transMtx[pathIdx],60);
	}

	for (pathIdx=0;pathIdx<60;pathIdx++) prevPathProbs[pathIdx]=pathProbs[pathIdx];

	maxProb=0.0;
	maxProbIdx=-1;
	for (chordIdx=0;chordIdx<60;chordIdx++) {
		maxPathProbIdx=-1;
		maxPathProb=0.0;
		for (pathIdx=0;pathIdx<60;pathIdx++) {
			if (transMtx[pathIdx][chordIdx]<=0.0) prob=prevPathProbs[pathIdx]-100.0;
			else prob=pathProbs[pathIdx]+log(transMtx[pathIdx][chordIdx]);
			if (pathIdx==0||prob>maxPathProb) {
				maxPathProb=prob;
				maxPathProbIdx=pathIdx;
			}
		}
		pathProbs[chordIdx]=maxPathProb;
		if (maxPathProbIdx==chordIdx) { //same chord as before
			pathChordLengths[chordIdx][0]++;
		}
		else { //new chord
			pathChordLengths[chordIdx][1]=pathChordLengths[chordIdx][0];
			pathChordLengths[chordIdx][0]=1;
		}
		if (chordIdx==0||maxPathProb>maxProb) {
			maxProb=maxPathProb;
			maxProbIdx=chordIdx;
		}
	}

	return maxProbIdx;
}

bool Viterbi::normalize(float *chroma,int n) {
	//normalizes chroma so sum of chroma==1
	//if all vals=0 set chroma to array of 1/12 and return 0
	//else return 1
	int i;
	float total=0.0;

	for (i=0;i<n;i++) total+=chroma[i];
	if (total==0) {
		for (i=0;i<n;i++) chroma[i]=1.0/n;
		return false;
	}
	else {
		for (i=0;i<n;i++) chroma[i]/=total;
		return true;
	}
}

// ------------- bass chroma -----------------
void Viterbi::getRootProbs(float *chroma, float *rootProbs) {
	//finds probability that each note is the root and places it in rootProbs array

	int i;
	int tableIdx[12];
	float chromaTotal=0.0;
	bool success;

	success=normalize(chroma,12);
	if (!success) {
		for (i=0;i<12;i++) rootProbs[i]=1.0/12.0;
		return;
	}

	for (i=0;i<12;i++) {
		tableIdx[i]=(int)ceil(10*chroma[i]/chromaTotal);
	}
	for (i=0;i<12;i++) {
		rootProbs[i]=bassProbTable[tableIdx[i]][tableIdx[(i+7)%12]];
	}
	normalize(rootProbs,12);
}

// ------------------ mid chroma ----------------------
float Viterbi::getMidChordProb(float *chroma, int chordTypeId, int noteNum) {
	//finds probability of chord with root note noteNum and type chordTypeId
	//by finding the product pdf values of each of the 12 pitches for that chord

	float prob=1.0;
	int i;
	int j;
	float x,y,mu,sigma,d,a1,a2;

	for (i=0;i<12;i++) {
		j=(i+noteNum)%12;
		x=chroma[j];
		if (x<0.01) y=chordZeroTable[chordTypeId][i];
		else {
			a1=chordGaussTable[chordTypeId][i][0];
			mu=chordGaussTable[chordTypeId][i][1];
			sigma=chordGaussTable[chordTypeId][i][2];
			a2=chordGaussTable[chordTypeId][i][3];
			d=(x-mu)/sigma;
			y=a1*exp(-0.5*d*d)+a2*exp(-7*fabs(x-mu));
		}
		prob*=y;
	}
	return prob;
}

void Viterbi::getChordProbs(float *bassChroma, float *midChroma, float *chordProbs) {
	int i,j;
	float rootProbs[12];

	normalize(midChroma,12);
	getRootProbs(bassChroma,rootProbs);

	for (i=0;i<5;i++) {
		for (j=0;j<12;j++) {
			chordProbs[i*12+j]=rootProbs[j]*getMidChordProb(midChroma,i,j)*chordTypeProbs[i];
		}
	}

	normalize(chordProbs,60);
}
