#ifndef LR2_SECONDARYSELECTIONWIDGETS_H
#define LR2_SECONDARYSELECTIONWIDGETS_H
#include "decomp.h"
#include "types.h"
class BitmapFont;
struct GlobalOverlaySprite;
struct SecondarySelectionSpriteDefinition {
	const LegoChar* m_name;
	LegoFloat m_rectangle[4];
	LegoFloat m_tileSize[2];
	LegoU32 m_color;
	const LegoChar* m_texturePath;
	LegoFloat m_textureRegion[4];
	LegoU32 m_unk0x34, m_unk0x38, m_unk0x3c;
	LegoS32 m_list;
};
DECOMP_SIZE_ASSERT(SecondarySelectionSpriteDefinition, 0x44);
struct SecondarySelectionCategoryDefinition {
	LegoU32 m_values[4];
};
DECOMP_SIZE_ASSERT(SecondarySelectionCategoryDefinition, 0x10);
struct SecondarySelectionItemDefinition {
	LegoU32 m_values[17];
};
DECOMP_SIZE_ASSERT(SecondarySelectionItemDefinition, 0x44);
void __fastcall InitializeSecondarySpriteSet(void*, void*, const SecondarySelectionSpriteDefinition*, LegoS32);
void __fastcall ShowSecondarySpriteSet(void*, void*);
void __fastcall HideSecondarySpriteSet(void*, void*);
void __fastcall ReleaseSecondarySpriteSet(void*, void*);
void __fastcall InitializeSecondarySelectionGrid(void*, void*);
void __fastcall DefineSecondarySelectionCategory(void*, void*, const SecondarySelectionCategoryDefinition*);
void __fastcall AddSecondarySelectionItem(void*, void*, const SecondarySelectionItemDefinition*, LegoU32);
void __fastcall ReleaseSecondarySelectionGrid(void*, void*);
void __fastcall FinalizeSecondarySelectionGrid(void*, void*);
void __fastcall HideSecondarySelectionGrid(void*, void*);
void __fastcall SetSecondarySelectionGridFont(void*, void*, BitmapFont*);
GlobalOverlaySprite* __fastcall FindSecondarySprite(void*, void*, const LegoChar*);
#endif
