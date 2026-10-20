#include "game/objects/gameobjectfactoryregistry.h"

#include "core/debug/diagnostics.h"
#include "frontend/preview/cardisplay.h"
#include "game/objects/gameobjectdispatch.h"

#include <string.h>

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

static const LegoChar* __fastcall GetRegisteredGameObjectFactoryClassName(GameObjectFactoryEntry*, void*);

// Alias behavior at 0x0040ec90; its bytes overlap Application::GetDisplaySettings.
static const LegoChar* __fastcall GetRegisteredGameObjectFactoryClassName(GameObjectFactoryEntry* p_factory, void*)
{
	return p_factory->m_className.m_data;
}

extern "C" int __cdecl _purecall(void);

// Retail table VA: 0x00536318; the second slot is the CRT pure-call handler.
extern const GameObjectFactoryDispatch g_gameObjectFactoryEntryDispatch;
const GameObjectFactoryDispatch g_gameObjectFactoryEntryDispatch = {
	&GetRegisteredGameObjectFactoryClassName,
	(FactoryConstruct) &_purecall,
	(FactoryZero) &GameObjectBaseZero,
	(FactoryFourArgumentZero) &GameObjectFactoryReturnZeroForFourArguments
};

// Retail dispatch table VA: 0x00536cf8
extern const GameObjectFactoryDispatch g_surfaceManagerFactoryDispatch;
const GameObjectFactoryDispatch g_surfaceManagerFactoryDispatch = {
	&GetRegisteredGameObjectFactoryClassName,
	(FactoryConstruct) &ConstructCarDisplayIndexedRecordOwner,
	(FactoryZero) &GameObjectBaseZero,
	(FactoryFourArgumentZero) &GameObjectFactoryReturnZeroForFourArguments
};

struct SurfaceManagerFactoryRegistration : GameObjectFactoryEntry {
	SurfaceManagerFactoryRegistration() : GameObjectFactoryEntry("cSurfaceManager")
	{
		m_vtable = &g_surfaceManagerFactoryDispatch;
	}
};

// FUNCTION: LEGORACERS2 0x0040eca0
LegoU32 __fastcall GameObjectFactoryReturnZeroForFourArguments(
	GameObjectFactoryEntry*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
)
{
	return 0;
}

// GLOBAL: LEGORACERS2 0x0067a1d0
static GameObjectFactoryEntry* g_registeredGameObjectFactories[81];

// GLOBAL: LEGORACERS2 0x0067a314
static LegoU32 g_registeredGameObjectFactoryCount;

// GLOBAL: LEGORACERS2 0x006843c0
static SurfaceManagerFactoryRegistration g_surfaceManagerFactoryRegistration;

// GLOBAL: LEGORACERS2 0x0067a1cc
GameObjectFactoryRegistry g_gameObjectFactoryRegistry;

// STUB: LEGORACERS2 0x004bd0d0
void GameObjectFactoryRegistry::Register(GameObjectFactoryEntry* p_factory)
{
	LegoU32 index = 0;
	while (index < g_registeredGameObjectFactoryCount) {
		GameObjectFactoryEntry* existing = g_registeredGameObjectFactories[index];
		LegoS32 order = _stricmp(p_factory->GetClassName(), existing->GetClassName());
		if (order < 0) {
			break;
		}
		++index;
	}
	if (index < g_registeredGameObjectFactoryCount) {
		for (LegoU32 move = g_registeredGameObjectFactoryCount; move > index; --move) {
			g_registeredGameObjectFactories[move] = g_registeredGameObjectFactories[move - 1];
		}
	}
	g_registeredGameObjectFactories[index] = p_factory;
	++g_registeredGameObjectFactoryCount;
}

// STUB: LEGORACERS2 0x004bd150
const GameObjectFactoryEntry* GameObjectFactoryRegistry::Find(const LegoChar* p_className) const
{
	LegoU32 low = 0;
	LegoU32 high = g_registeredGameObjectFactoryCount - 1;
	do {
		LegoU32 middle = (high + low) >> 1;
		GameObjectFactoryEntry* entry = g_registeredGameObjectFactories[middle];
		LegoS32 order = _stricmp(p_className, entry->GetClassName());
		if (order < 0) {
			high = middle - 1;
		}
		else if (order == 0) {
			return entry;
		}
		else {
			low = middle + 1;
		}
	} while (low <= high);
	return NULL;
}

// STUB: LEGORACERS2 0x004bd1b0
GameObject* GameObjectFactoryRegistry::ConstructByClassName(
	const LegoChar* p_className,
	LegoU32 p_argument,
	GameObjectTemplate* p_template,
	LegoBool32 p_reportUnknownClass
) const
{
	const GameObjectFactoryEntry* entry = Find(p_className);
	if (entry != NULL) {
		return ((GameObjectFactoryEntry*) entry)->Construct(p_argument, p_template);
	}
	if (p_reportUnknownClass != FALSE) {
		ReportFatalError("Unknown object class name '%s'", p_className);
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004bd200
GameObjectFactoryEntry::GameObjectFactoryEntry(const LegoChar* p_className)
	: m_vtable(&g_gameObjectFactoryEntryDispatch), m_className(p_className)
{
	g_gameObjectFactoryRegistry.Register(this);
}

const LegoChar* GameObjectFactoryEntry::GetClassName() const
{
	return m_vtable->m_getClassName((GameObjectFactoryEntry*) this, NULL);
}

GameObject* GameObjectFactoryEntry::Construct(LegoU32 p_argument, GameObjectTemplate* p_template)
{
	return m_vtable->m_construct((GameObjectFactoryEntry*) this, NULL, p_argument, p_template);
}
