#ifndef CHORDDETECTUI_H
#define CHORDDETECTUI_H

#ifndef __vstgui__
#include "vstgui.h"
#endif

#define BACKGROUND_ID 128

class ChordDetectUI : public AEffGUIEditor//, public CControlListener
{
	public:
		ChordDetectUI(AudioEffect* effect);
		virtual ~ChordDetectUI();

		virtual bool open (void* ptr);
		virtual void close ();
		void updateChord(const char *newChord);
		//virtual void valueChanged (CDrawContext* context, CControl* control);
	protected:
	private:
		// Bitmap
		CBitmap* bgBmp;
		CTextLabel* chordText;
		char curChord[64];
};

#endif // CHORDDETECTUI_H
