#include "game/world/racestartposition.h"

#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gameobjectfactoryregistry.h"
#include "memory/memoryheap.h"

#include <new>
#include <string.h>

struct RaceStartPositionTemplateView {
	GameObjectTemplate m_base;
	undefined m_unk0x34[0x50 - 0x34];
};
DECOMP_SIZE_ASSERT(RaceStartPositionTemplateView, 0x50);

struct RaceStartPositionObjectView {
	GameObject m_base;
	undefined m_unk0x58[0x74 - 0x58];
};
DECOMP_SIZE_ASSERT(RaceStartPositionObjectView, 0x74);

typedef LegoU32(__fastcall* FactoryZero)(GameObjectFactoryEntry*, void*);
typedef GameObject*(__fastcall* FactoryConstruct)(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);
typedef LegoU32(__fastcall* FactoryFourArgumentZero)(
	GameObjectFactoryEntry*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
);

struct RaceStartPositionFactoryDispatch {
	const LegoChar*(__fastcall* m_getClassName)(GameObjectFactoryEntry*, void*);
	GameObject*(__fastcall* m_construct)(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);
	LegoU32(__fastcall* m_unk0x08)(GameObjectFactoryEntry*, void*);
	LegoU32(__fastcall* m_unk0x0c)(GameObjectFactoryEntry*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
};
DECOMP_SIZE_ASSERT(RaceStartPositionFactoryDispatch, 0x10);

static const LegoChar* __fastcall GetRaceStartPositionFactoryClassName(GameObjectFactoryEntry* p_factory, void*)
{
	return p_factory->m_className.m_data;
}

struct RaceStartPositionObjectDispatch {
	GameObjectDispatchView m_base;
	LegoU32(__fastcall* m_getAllocationSize)(GameObject*, void*);
};
DECOMP_SIZE_ASSERT(RaceStartPositionObjectDispatch, 0x54);
static LegoU32 __fastcall GetRaceStartPositionBaseSize(GameObject*, void*);
static LegoU32 __fastcall GetRaceStartPositionSize(GameObject*, void*);
// Retail ABI function-pointer table: 0x0052f848, twenty-one slots.
extern const RaceStartPositionObjectDispatch g_raceStartPositionBaseObjectDispatch;
const RaceStartPositionObjectDispatch g_raceStartPositionBaseObjectDispatch = {
	GAME_OBJECT_BASE_DISPATCH_INITIALIZER,
	&GetRaceStartPositionBaseSize
};
// Retail ABI function-pointer table: 0x0052f7f4, twenty-one slots.
extern const RaceStartPositionObjectDispatch g_raceStartPositionObjectDispatch;
const RaceStartPositionObjectDispatch g_raceStartPositionObjectDispatch = {
	GAME_OBJECT_BASE_DISPATCH_INITIALIZER,
	&GetRaceStartPositionSize
};

// Retail factory table VA: 0x0052f7e4.
extern const RaceStartPositionFactoryDispatch g_raceStartPositionFactoryDispatch;
const RaceStartPositionFactoryDispatch g_raceStartPositionFactoryDispatch = {
	&GetRaceStartPositionFactoryClassName,
	(FactoryConstruct) &ConstructRaceStartPosition,
	(FactoryZero) &GameObjectBaseZero,
	(FactoryFourArgumentZero) &GameObjectFactoryReturnZeroForFourArguments
};

struct RaceStartPositionFactoryRegistration : GameObjectFactoryEntry {
	RaceStartPositionFactoryRegistration() : GameObjectFactoryEntry("cRaceStartPos")
	{
		m_vtable = (const GameObjectFactoryDispatch*) &g_raceStartPositionFactoryDispatch;
	}
};

// GLOBAL: LEGORACERS2 0x0055e248
static RaceStartPositionFactoryRegistration g_raceStartPositionFactoryRegistration;

// STUB: LEGORACERS2 0x0040ecb0
GameObject* __fastcall ConstructRaceStartPosition(void*, void*, LegoU32, GameObjectTemplate* p_template)
{
	if ((p_template->m_flags & 4) == 0) {
		p_template->m_flags |= 4;
	}

	void* storage = AllocateHeapMemory(sizeof(RaceStartPositionObjectView));
	if (storage == NULL) {
		return NULL;
	}

	GameObject* object = new (storage) GameObject(0x900, p_template, NULL);
	object->m_vtable = &g_raceStartPositionBaseObjectDispatch.m_base;
	memcpy((LegoU8*) object + 0x58, (LegoU8*) p_template + 0x34, 0x0c);
	object->m_vtable = &g_raceStartPositionObjectDispatch.m_base;
	memcpy((LegoU8*) object + 0x64, (LegoU8*) p_template + 0x40, 0x10);
	return object;
}

// FUNCTION: LEGORACERS2 0x0040ed40
static LegoU32 __fastcall GetRaceStartPositionBaseSize(GameObject*, void*)
{
	return 0x64;
}
// FUNCTION: LEGORACERS2 0x0040ed50
static LegoU32 __fastcall GetRaceStartPositionSize(GameObject*, void*)
{
	return 0x74;
}
