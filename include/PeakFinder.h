#ifndef PEAKFINDER_H
#define PEAKFINDER_H

#define LOG2 0.6931471805599453
#include <vector>

struct Peak {
	float fq; //frequency in bin index
	float amp;
	Peak() : fq(0), amp(0) {}
	Peak(float fq, float amp) : fq(fq), amp(amp) {}
};

bool peakCompare (Peak const obj1, Peak const obj2);

class PeakFinder
{
	public:
		PeakFinder();
		void getChromas(float bassChroma[], float midChroma[],std::vector<float> spec,float sampleRate);
		std::vector<Peak> findPeaks(std::vector<float> spec);
		virtual ~PeakFinder();
	protected:
		Peak quadSpline(float yprev,float y0,float ynext);
	private:
		void pruneMaxes(std::vector<Peak> peakArr,float semiDist,float minAmpRatio);
		float errorPenalty(float errorAmt);
};

#endif // PEAKFINDER_H
