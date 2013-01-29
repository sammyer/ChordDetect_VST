#include "PeakFinder.h"
#include <Math.h>
#include <algorithm>
#include <vector>
#include <cstdio>

PeakFinder::PeakFinder()
{
	//ctor
}

PeakFinder::~PeakFinder()
{
	//dtor
}

bool peakCompare (Peak const obj1, Peak const obj2) {
	return obj1.amp<obj2.amp;
}

Peak PeakFinder::quadSpline(float yprev,float y0,float ynext) {
	//interpolate with a quadratic spline
	float a,b,c,x,y;
	c=y0;
	b=0.5*(ynext-yprev);
	a=0.5*(ynext+yprev)-y0;
	if (a==0) x=0;
	else x=-0.5*b/a;
	y=a*x*x+b*x+c;
	return Peak(x,y);
}

std::vector<Peak> PeakFinder::findPeaks(std::vector<float> spec) {
	int i;
	int n=spec.size();
	int peakIdx;
	float minVal;
	float prevMin=0.0;
	std::vector<Peak> peaks;
	peaks.reserve(2048);
	Peak newPeak;

	//find local maxima as peaks
	//also find minimum values between those local maxima, assume this is noise, and subtract from peak values
	peakIdx=0;
	minVal=-1.0; //initialize value to negative to represent no minimum found yet
	for (i=0;i<n;i++) {
		if (i>0&&i<n-1&&spec[i]>spec[i-1]&&spec[i]>spec[i+1]) {
			//peak found - do quadratic interpolation
			newPeak=quadSpline(spec[i-1],spec[i],spec[i+1]);
			newPeak.fq+=i;
			peaks.push_back(newPeak);

			//subtract local minima on either side of each maximum in order to compensate for noise
			// peakIdx-1 because we are here :
			// peals[peakIdx-2] .... prevMin ....  peaks[peakIdx-1] .... minVal ..... HERE
			if (peakIdx>0) peaks[peakIdx-1].amp-=0.5*(minVal+prevMin);

			prevMin=minVal;
			minVal=-1.0;
			peakIdx++;
		}
		else if (minVal<0) { //first value of minimum
			minVal=spec[i];
		}
		else { //find minimum value between peaks
			if (spec[i]<minVal) minVal=spec[i];
		}
	}
	if (peakIdx>0) peaks[peakIdx-1].amp-=0.5*(minVal+prevMin);

	return peaks;
}

void PeakFinder::pruneMaxes(std::vector<Peak> peakArr,float semiDist,float minAmpRatio) {
	//prunes peaks as follow:
	//semiDist - peaks must be at least semiDist semitones apart
	//ampDiff - peak amp must be >max(peak.amp)*minAmpRatio
	//maximum number of peaks returned=50
	int maxN=50;

	std::vector<bool> isMaxRemoved(peakArr.size(),false);
	int peaksKept=0;
	int i;
	int j;
	int n;
	float minAmp;
	int minAmpIdx; //index at which amp goes below threshold - only need to check up to this index
	float fqRatio=pow(2.0,semiDist/12.0);

	std::sort(peakArr.begin(),peakArr.end(),peakCompare);

	minAmp=peakArr[0].amp*minAmpRatio;
	n=peakArr.size();
	minAmpIdx=n;
	for (i=0;i<n;i++) {
		if (peakArr[i].amp<minAmp) {
			minAmpIdx=i;
			break;
		}
	}

	for (j=0;j<minAmpIdx;j++) {
		if (isMaxRemoved[j]) continue;
		for (i=j+1;i<minAmpIdx;i++) {
			if (isMaxRemoved[i]) continue;
			if (peakArr[i].fq<peakArr[j].fq*fqRatio&&peakArr[i].fq*fqRatio>peakArr[j].fq) {
				isMaxRemoved[i]=1;
			}
		}
		peaksKept++;
		if (peaksKept==maxN) break;
	}

	//consolidate array by overwriting peaks no longer used
	i=0;
	j=0;
	for (i=0;i<n;i++) {
		if (isMaxRemoved[i]) continue;
		if (i!=j) {
			peakArr[j].fq=peakArr[i].fq;
			peakArr[j].amp=peakArr[i].amp;
		}
		j++;
		if (j==peaksKept) break;
	}
	peakArr.resize(peaksKept);
}

float PeakFinder::errorPenalty(float errorAmt) {
	//return cos(errorAmt*PI)
	float abserr=fabs(errorAmt);
	if (abserr>0.25) return 2.0-abserr*4.0;
	else return 1.0;
}


void PeakFinder::getChromas(float bassChroma[], float midChroma[],std::vector<float> spec,float sampleRate) {
	//float chroma[2][12];
	int i;
	int numPeaks;
	float semis;
	std::vector<Peak> peakArr;
	int n=spec.size();
	int windowSize=(n-1)*2;
	int chromaId;

	int pitchId; //where 0=C 1=C# ... 11=B
	float tuningError;

	//precalculate these 2 values
	float n1=sampleRate/windowSize/440.0;
	float n2=12.0/LOG2;

	peakArr=findPeaks(spec);
	pruneMaxes(peakArr,0.4,0.002);
	//printf("%d, ",numPeaks);

	for (i=0;i<12;i++) {
		bassChroma[i]=0;
		midChroma[i]=0;
	}

	numPeaks=peakArr.size();
	for (i=0;i<numPeaks;i++) {
		//semitones from A440 = log2(fq*fsamp/windowsize)*12
		semis=log(peakArr[i].fq*n1)*n2;
		if (semis>=-40&&semis<-16) chromaId=0; //bass
		else if (semis>=-16&&semis<8) chromaId=1; //mids
		else continue; //outside of range

		pitchId=(int)round(semis);
		pitchId=(pitchId+57)%12;
		tuningError=semis-round(semis);
		if (chromaId==0) bassChroma[pitchId]+=peakArr[i].amp*errorPenalty(tuningError);
		if (chromaId==1) midChroma[pitchId]+=peakArr[i].amp*errorPenalty(tuningError);
	}
}
