#ifndef LR2_APPLICATIONMESSAGES_H
#define LR2_APPLICATIONMESSAGES_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>
#include <windows.h>

class ApplicationCharacterQueue {
public:
	void QueueCharacter(LegoU32 p_character);

	LegoU16 m_characters[64]; // 0x00: queued WM_CHAR code units
	LegoU32 m_writeIndex;     // 0x80
	LegoU32 m_readIndex;      // 0x84
	LegoU32 m_busy;           // 0x88: guards against reentrant queue access
};

DECOMP_STATIC_ASSERT(offsetof(ApplicationCharacterQueue, m_writeIndex) == 0x80);
DECOMP_STATIC_ASSERT(offsetof(ApplicationCharacterQueue, m_readIndex) == 0x84);
DECOMP_STATIC_ASSERT(offsetof(ApplicationCharacterQueue, m_busy) == 0x88);

// Sparse projection of fields used by the Application message filter. The
// reserved byte ranges are observed offsets, not recovered member arrays.
struct GameApplicationMessageContextView {
	LegoU8 m_unk0x00[0x60];
	LegoU32 m_inputMode;          // 0x60
	LegoU32 m_alternateInputMode; // 0x64
	LegoU8 m_unk0x68[0x0d58];
	ApplicationCharacterQueue m_characterQueue; // 0xdc0
	LegoU8 m_unk0xe4c[0x1c38];
	void* m_textInputTarget; // 0x2a84
};

extern GameApplicationMessageContextView* g_gameApplicationMessageContext;

LegoBool32 WINAPI FilterApplicationWindowMessage(HWND p_window, UINT p_message, WPARAM p_wParam, LPARAM p_lParam);

#endif // LR2_APPLICATIONMESSAGES_H
