#ifndef LR2_AUDIOSETTINGS_H
#define LR2_AUDIOSETTINGS_H

#include "types.h"

// Only the accessed prefix is known. The field at +0x88 is read as a DWORD;
// this projection makes no claim about the complete owning object.
struct AudioSettingsContextView {
	LegoU8 m_unk0x00[0x88];
	LegoU32 m_audioServiceMode;
};

LegoU32 GetConfiguredAudioServiceMode();

extern AudioSettingsContextView* g_audioSettingsContext;

#endif // LR2_AUDIOSETTINGS_H
