#ifndef LR2_SECONDARYMANAGERAUDIONAMES_H
#define LR2_SECONDARYMANAGERAUDIONAMES_H

#include "charbuffer24.h"

extern LegoU32 g_secondaryManagerAudioNameStorage[60];

inline CharBuffer24* GetSecondaryManagerAudioNames()
{
	return reinterpret_cast<CharBuffer24*>(g_secondaryManagerAudioNameStorage);
}

#endif // LR2_SECONDARYMANAGERAUDIONAMES_H
