#ifndef CHORD_H
#define CHORD_H

#include <string>
#define NUM_CHORDS 60

using namespace std;

const char chordNames[5][4]={"","m","dim","aug","sus"};
const char noteNames[12][3]={"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};

class Chord
{
	public:
		Chord();
		Chord(int chordId);
		virtual ~Chord();

		int getChordRootId();
		int getChordTypeId();
		string getChordName();
	protected:
	private:
		int rootId; // 12 root notes from C,C#...B
		int typeId; // 5 chord type: major, minor, diminished, augmented, suspended
};

#endif // CHORD_H
