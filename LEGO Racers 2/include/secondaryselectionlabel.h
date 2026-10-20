#ifndef LR2_SECONDARYSELECTIONLABEL_H
#define LR2_SECONDARYSELECTIONLABEL_H

#include "decomp.h"
#include "types.h"

class BitmapFont;
struct GlobalOverlayRecord;

struct SecondarySelectionLabel {
	GlobalOverlayRecord* m_record;
	LegoFloat m_width;
	LegoFloat m_scale;
};
DECOMP_SIZE_ASSERT(SecondarySelectionLabel, 0x0c);

void* __fastcall ConstructSecondarySelectionLabel(void*, void*);
void __fastcall LoadSecondarySelectionLabel(void*, void*);
void __fastcall ReleaseSecondarySelectionLabel(void*, void*);
void __fastcall SetSecondarySelectionLabelFont(void*, void*, BitmapFont*, LegoFloat);

#endif
