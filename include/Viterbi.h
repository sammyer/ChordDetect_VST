#ifndef VITERBI_H
#define VITERBI_H


class Viterbi
{
	public:
		Viterbi();
		virtual ~Viterbi();
		int viterbi(float *bassChroma, float *midChroma);
	protected:
	private:
		float pathProbs[60];
		int pathChordLengths[60][2];

		float getTransProb(int chordLen, int prevChordLen);
		bool normalize(float *chroma,int n);
		void getRootProbs(float *chroma, float *rootProbs);
		float getMidChordProb(float *chroma, int chordTypeId, int noteNum);
		void getChordProbs(float *bassChroma, float *midChroma, float *chordProbs);

};

#endif // VITERBI_H
