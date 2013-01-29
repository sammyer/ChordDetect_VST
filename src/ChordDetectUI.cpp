#include "ChordDetectUI.h"

ChordDetectUI::ChordDetectUI(AudioEffect *effect)
 : AEffGUIEditor (effect)
{
	// load the background bitmap
	// we don't need to load all bitmaps, this could be done when open is called
	bgBmp = new CBitmap (BACKGROUND_ID);

	// init the size of the plugin
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = (short)bgBmp->getWidth ();
	rect.bottom = (short)bgBmp->getHeight ();
	//rect.right  = 155;
	//rect.bottom = 155;
	strcpy(curChord,"");
}

ChordDetectUI::~ChordDetectUI()
{
	//dtor
	if (bgBmp) bgBmp->forget();
	bgBmp = NULL;
}

bool ChordDetectUI::open (void *ptr)
{
	// !!! always call this !!!
	AEffGUIEditor::open (ptr);

	//--init background frame-----------------------------------------------
	// We use a local CFrame object so that calls to setParameter won't call into objects which may not exist yet.
	// If all GUI objects are created we assign our class member to this one. See bottom of this method.
	CRect size (0, 0, bgBmp->getWidth (), bgBmp->getHeight ());
	CFrame* lFrame = new CFrame (size, ptr, this);
	lFrame->setBackground (bgBmp);

	//size (kDisplayX, kDisplayY, kDisplayX + kDisplayXWidth, kDisplayY + kDisplayHeight);
	chordText=new CTextLabel(size,curChord);
	chordText->setTransparency(true);
	CColor textColor={100,120,255,255};
	chordText->setFontColor(textColor);
	chordText->setFontSize(60);
	chordText->setFont(kNormalFontVeryBig);
	lFrame->addView(chordText);

	frame = lFrame;
	return true;
}

void ChordDetectUI::updateChord(const char *newChord) {
	strcpy(curChord,newChord);
	if (!frame) return;
	chordText->setText(newChord);
	CRect size;
	frame->getSize(size);
	frame->invalidate(size);
}

// void ChordDetectUI::valueChanged (CDrawContext* context, CControl* control) {}

//-----------------------------------------------------------------------------
void ChordDetectUI::close ()
{
	delete frame;
	frame = 0;
}
