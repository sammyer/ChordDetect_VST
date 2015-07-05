#include "Chord.h"
#include <sstream>


namespace chords {

Chord::Chord(): rootId(0), typeId(0) {
}

Chord::Chord(int chordId):
	rootId(chordId%12),
	typeId(chordId/12)
	{}

Chord::~Chord()
{
	//dtor
}

int Chord::getChordRootId() {
	return rootId;
}

int Chord::getChordTypeId() {
	return typeId;
}

std::string Chord::getChordName() {
	std::ostringstream output;
	output << noteNames[rootId] << chordNames[typeId];
	return output.str();
}

}
