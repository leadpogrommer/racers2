#ifndef LR2_SECONDARYMANAGERLEASES_H
#define LR2_SECONDARYMANAGERLEASES_H

#include "secondarymanager.h"

// The manager retains ownership; callers receive a pointer plus a lease key.
// A key of -1 marks a free slot, and successful keys may be zero.
LegoS32 __fastcall AcquireMenuPreviewCamera(SecondaryManager*, void*, void**);
void __fastcall ReleaseMenuPreviewCamera(SecondaryManager*, void*, void**, LegoS32);
LegoS32 __fastcall AcquireMenuPreviewCharacter(SecondaryManager*, void*, void**);
void __fastcall ReleaseMenuPreviewCharacter(SecondaryManager*, void*, void**, LegoS32);
LegoS32 __fastcall AcquireMenuPreviewAmbient(SecondaryManager*, void*, void**);
void __fastcall ReleaseMenuPreviewAmbient(SecondaryManager*, void*, void**, LegoS32);
LegoS32 __fastcall AcquireMenuPreviewLight(SecondaryManager*, void*, void**);
void __fastcall ReleaseMenuPreviewLight(SecondaryManager*, void*, void**, LegoS32);

#endif
