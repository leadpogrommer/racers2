#include "npctextmanager.h"

#include "foyerselectionhelpers.h"
#include "gamecontroller.h"
#include "gameobjectdispatch.h"
#include "globaloverlaymanager.h"
#include "localizedstringtable.h"
#include "memoryheap.h"
#include "textboxmanager.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x00530a04
static LegoFloat g_npcTextCharacterInterval = 0.1f;

LegoBool32 __fastcall FindNpcTextStringByName(LocalizedStringTable*, void*, const LegoChar*, const LegoU16**);

void __fastcall InitializeNestedNpcTextBox(
	NestedTextBoxManagerPrefix*,
	void*,
	const LegoU16*,
	const LegoU32*,
	const LegoU32*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
); // 00437dc0

// STUB: LEGORACERS2 0x00438fd0
GameObject* __fastcall ConfigureNpcTextBox(
	GameObject* p_manager,
	void*,
	const LegoU16* p_text,
	const LegoU32* p_firstMetrics,
	const LegoU32* p_secondMetrics,
	LegoU32 p_mode,
	LegoU32 p_textStyle,
	LegoU32 p_flags,
	LegoU32 p_style
)
{
	TextBoxManager* manager = (TextBoxManager*) p_manager;
	NestedTextBoxManagerPrefix* nested = (NestedTextBoxManagerPrefix*) manager->m_nestedManager;
	if (nested->m_active != 0) {
		return NULL;
	}

	InitializeNestedNpcTextBox(
		nested,
		NULL,
		p_text,
		p_firstMetrics,
		p_secondMetrics,
		p_mode,
		p_textStyle,
		p_flags,
		p_style
	);
	return (GameObject*) nested;
}

// STUB: LEGORACERS2 0x00439050
static void __fastcall SetNpcTextCharacterInterval(GameObject* p_object, void*, LegoFloat p_interval)
{
	GameObject* nested = ((TextBoxManager*) p_object)->m_nestedManager;
	*(LegoFloat*) ((LegoU8*) nested + 0x18dc) = p_interval;
}

// STUB: LEGORACERS2 0x00439060
LegoFloat __fastcall GetNpcTextCharacterInterval(GameObject*, void*)
{
	return g_npcTextCharacterInterval;
}

struct NpcTextManagerDispatch {
	GameObjectDispatchView m_object;
	GameObjectDispatchView::Value m_size;
};
static GameObject* __fastcall DestroyNpcTextManager(GameObject*, void*, LegoU32);
static LegoU32 __fastcall GetNpcTextManagerSize(GameObject*, void*);
static void __fastcall InitializeNpcTextManager(GameObject*, void*);
static LegoU32 __fastcall UpdateNpcTextManager(GameObject*, void*, LegoU32);
// Retail object table0x00532628, complete21slotinventory.
extern const NpcTextManagerDispatch g_npcTextManagerDispatch = {
	{&DestroyNpcTextManager,      &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &InitializeNpcTextManager,   &GameObjectBaseTrue,
	 &UpdateNpcTextManager,       &GameObjectBaseRemove,
	 &GameObjectBaseZero,         &GameObjectBaseZero,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	 &GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument},
	&GetNpcTextManagerSize
};
// STUB: LEGORACERS2 0x0045f9e0
NpcTextManager::NpcTextManager(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_nestedTextManager = NULL;
	m_selectedIndex = -1;
	m_unk0x6c = 0;
	m_vtable = &g_npcTextManagerDispatch.m_object;
}
// FUNCTION: LEGORACERS2 0x0045fa20
static LegoU32 __fastcall GetNpcTextManagerSize(GameObject*, void*)
{
	return 0x70;
}
// FUNCTION: LEGORACERS2 0x0045fa30
static GameObject* __fastcall DestroyNpcTextManager(GameObject* p_object, void*, LegoU32 p_flags)
{
	((NpcTextManager*) p_object)->~NpcTextManager();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}
// STUB: LEGORACERS2 0x0045fa50
NpcTextManager::~NpcTextManager()
{
	m_vtable = &g_npcTextManagerDispatch.m_object;
}
// STUB: LEGORACERS2 0x0045fa60
static void __fastcall InitializeNpcTextManager(GameObject* p_object, void*)
{
	p_object->Initialize();
	((NpcTextManager*) p_object)->m_nestedTextManager = g_gameController.m_textBoxManager;
}
// STUB: LEGORACERS2 0x0045fa80
static LegoU32 __fastcall UpdateNpcTextManager(GameObject* p_object, void*, LegoU32 p_frame)
{
	NpcTextManager* manager = (NpcTextManager*) p_object;
	if (manager->m_selectedIndex == -1 || !(manager->m_value >= 0.0f)) {
		return 0;
	}
	union {
		LegoU32 m_word;
		LegoFloat m_float;
	} elapsed;
	elapsed.m_word = p_frame;
	manager->m_value = manager->m_value - elapsed.m_float * *(const LegoFloat*) &manager->m_unk0x68;
	if (!(manager->m_value > 0.0f)) {
		ClearSecondaryNpcTextMode(p_object, NULL, manager->m_selectedIndex);
	}
	return 1;
}

// STUB: LEGORACERS2 0x0045fe80
LegoU32 __cdecl CountNpcTextDisplayCharacters(const LegoU16* p_text)
{
	LegoU32 length = 0;
	while (p_text[length] != 0) {
		++length;
	}
	return length + 5;
}

// STUB: LEGORACERS2 0x0045feb0
void __fastcall SetNpcTextManagerTextByName(
	GameObject* p_object,
	void*,
	LegoU32 p_mode,
	const LegoChar* p_name,
	LegoFloat p_duration,
	LegoU32 p_style
)
{
	NpcTextManager* manager = (NpcTextManager*) p_object;
	LegoU8* controllerTextData = (LegoU8*) g_gameController.m_unk0x88 + 0x64;
	LegoU32 firstMetrics[2];
	LegoU32 secondMetrics[2];
	memcpy(firstMetrics, controllerTextData + 10 * 8, sizeof(firstMetrics));
	memcpy(secondMetrics, controllerTextData + 11 * 8, sizeof(secondMetrics));
	ClearFoyerNpcText(p_object, NULL);
	const LegoU16* text = NULL;
	FindNpcTextStringByName(g_gameController.m_localizedStrings, NULL, p_name, &text);
	manager->m_value = p_duration;
	manager->m_selectedIndex = p_mode;
	manager->m_textObject = ConfigureNpcTextBox(
		manager->m_nestedTextManager,
		NULL,
		text,
		firstMetrics,
		secondMetrics,
		p_mode,
		manager->m_unk0x6c,
		2,
		p_style
	);
	LegoFloat interval = GetNpcTextCharacterInterval(manager->m_nestedTextManager, NULL);
	LegoS32 count = (LegoS32) CountNpcTextDisplayCharacters(text);
	if (p_duration - 0.5f < (LegoFloat) count * interval) {
		SetNpcTextCharacterInterval(manager->m_nestedTextManager, NULL, (p_duration - 0.5f) / (LegoFloat) count);
	}
}

// STUB: LEGORACERS2 0x0045ffb0
void __fastcall SetNpcTextManagerText(
	GameObject* p_object,
	void*,
	LegoU32 p_mode,
	LegoS32 p_stringIndex,
	LegoFloat p_value,
	LegoU32 p_style
)
{
	NpcTextManager* manager = (NpcTextManager*) p_object;
	LegoU8* controllerTextData = (LegoU8*) g_gameController.m_unk0x88 + 0x64;
	const LegoU32* firstMetrics = (const LegoU32*) (controllerTextData + 10 * 8);
	const LegoU32* secondMetrics = (const LegoU32*) (controllerTextData + 11 * 8);
	const LegoU16* text = NULL;
	GameObject* textObject;

	ClearFoyerNpcText(p_object, NULL);
	GetLocalizedStringTable()->GetString(p_stringIndex, &text);
	manager->m_value = p_value;
	manager->m_selectedIndex = p_mode;
	textObject = ConfigureNpcTextBox(
		manager->m_nestedTextManager,
		NULL,
		text,
		firstMetrics,
		secondMetrics,
		p_mode,
		manager->m_unk0x6c,
		2,
		p_style
	);
	manager->m_textObject = textObject;
}

// STUB: LEGORACERS2 0x00460050
void __fastcall ClearSecondaryNpcTextMode(GameObject* p_object, void*, LegoU32 p_mode)
{
	if (((NpcTextManager*) p_object)->m_selectedIndex == (LegoS32) p_mode) {
		ClearFoyerNpcText(p_object, NULL);
	}
}

// STUB: LEGORACERS2 0x004b7910
LegoBool32 __fastcall FindNpcTextStringByName(
	LocalizedStringTable* p_table,
	void*,
	const LegoChar* p_name,
	const LegoU16** p_text
)
{
	LegoS32 low = 0;
	LegoS32 high = p_table->m_stringCount - 1;
	LegoS32 found = -1;
	while (low <= high && found == -1) {
		LegoS32 middle = low + ((high - low) >> 1);
		LegoS32 comparison = strcmp(p_name, p_table->m_order[middle].m_key);
		if (comparison == 0) {
			found = p_table->m_order[middle].m_stringIndex;
		}
		else if (comparison < 0) {
			high = middle - 1;
		}
		else {
			low = middle + 1;
		}
	}
	return p_table->GetString(found, p_text);
}
