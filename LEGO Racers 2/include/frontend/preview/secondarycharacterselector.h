#ifndef LR2_SECONDARYCHARACTERSELECTOR_H
#define LR2_SECONDARYCHARACTERSELECTOR_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

class BitmapFont;
struct GlobalOverlayRecord;
struct GlobalOverlaySprite;

struct SecondaryCharacterSelector {
	LegoS32 m_state;
	LegoS32 m_player;
	BitmapFont* m_font;
	LegoU8 m_spriteSet[0x404];
	LegoU8 m_selectionGrid[0x230c];
	void* m_previewCharacter; // 0x271c
	LegoS32 m_characterLease;
	void* m_previewCamera;
	LegoS32 m_cameraLease;
	void* m_previewAmbient;
	LegoS32 m_ambientLease;
	void* m_previewLights[2];
	LegoS32 m_lightLeases[2];
	LegoU8 m_animationState[0x18];
	GlobalOverlaySprite* m_sprites[2]; // 0x275c
	LegoU32 m_unk0x2764[2];
	LegoS32 m_backgroundTexture;
	BitmapFont* m_overlayFont;
	GlobalOverlayRecord* m_textRecords[5];
	LegoS32 m_bodypartIndices[4];
	LegoU32 m_unk0x2798[4];
};
DECOMP_SIZE_ASSERT(SecondaryCharacterSelector, 0x27a8);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCharacterSelector, m_selectionGrid) == 0x410);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCharacterSelector, m_previewCharacter) == 0x271c);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCharacterSelector, m_sprites) == 0x275c);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCharacterSelector, m_textRecords) == 0x2774);

void __fastcall LoadSecondaryCharacterSelector(void*, void*, BitmapFont*);
void __fastcall ReleaseSecondaryCharacterSelector(void*, void*);
void __fastcall FinishSecondaryCharacterSelection(void*, void*);

#endif
