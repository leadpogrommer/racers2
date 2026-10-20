#ifndef LR2_NPC_TEXT_MANAGER_H
#define LR2_NPC_TEXT_MANAGER_H

#include "gamemanager.h"

struct NpcTextSpeaker {
	LegoS32 m_stringIndex;
	LegoS32 m_portrait;
	LegoS32 m_voiceCategory;
	const LegoChar* m_name;
	LegoS32 m_characterIndex;
};
struct LocalizedStringTable;
DECOMP_SIZE_ASSERT(NpcTextSpeaker, 0x14);
extern const NpcTextSpeaker g_npcTextSpeakers[63];
LegoS32 __stdcall GetNpcTextPlayerPortrait(const LegoU32* p_parts);

struct NpcTextManager : GameObject {
	NpcTextManager(GameObjectTemplate*);
	~NpcTextManager();
	GameObject* m_nestedTextManager; // +0x58
	LegoS32 m_selectedIndex;         // +0x5c
	GameObject* m_textObject;        // +0x60
	LegoFloat m_value;               // +0x64
	LegoU32 m_unk0x68;
	LegoU32 m_unk0x6c;
};

DECOMP_SIZE_ASSERT(NpcTextManager, 0x70);
DECOMP_STATIC_ASSERT(offsetof(NpcTextManager, m_nestedTextManager) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(NpcTextManager, m_selectedIndex) == 0x5c);
DECOMP_STATIC_ASSERT(offsetof(NpcTextManager, m_textObject) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(NpcTextManager, m_value) == 0x64);

void __fastcall SetNpcTextManagerText(
	GameObject*,
	void*,
	LegoU32 p_mode,
	LegoS32 p_stringIndex,
	LegoFloat p_value,
	LegoU32 p_style
);
void __fastcall SetNpcTextManagerTextByName(GameObject*, void*, LegoU32, const LegoChar*, LegoFloat, LegoU32);
LegoU32 __cdecl CountNpcTextDisplayCharacters(const LegoU16*);
LegoFloat __fastcall GetNpcTextCharacterInterval(GameObject*, void*);
LegoBool32 __fastcall FindNpcTextStringByName(LocalizedStringTable*, void*, const LegoChar*, const LegoU16**);
void __fastcall ClearFoyerNpcText(GameObject*, void*);
void __fastcall ClearSecondaryNpcTextMode(GameObject*, void*, LegoU32);

#endif // LR2_NPC_TEXT_MANAGER_H
