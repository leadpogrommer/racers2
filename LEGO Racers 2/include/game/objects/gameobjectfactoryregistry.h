#ifndef LR2_GAMEOBJECTFACTORYREGISTRY_H
#define LR2_GAMEOBJECTFACTORYREGISTRY_H

#include "game/objects/gamemanager.h"

struct GameObjectFactoryEntry;

struct GameObjectFactoryDispatch {
	const LegoChar*(__fastcall* m_getClassName)(GameObjectFactoryEntry*, void*);
	GameObject*(__fastcall* m_construct)(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);
	LegoU32(__fastcall* m_noOp)(GameObjectFactoryEntry*, void*);
	LegoU32(__fastcall* m_zero)(GameObjectFactoryEntry*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
};

struct GameObjectFactoryEntry {
	const GameObjectFactoryDispatch* m_vtable;
	CharBuffer24 m_className;

	GameObjectFactoryEntry(const LegoChar* p_className);
	const LegoChar* GetClassName() const;
	GameObject* Construct(LegoU32 p_argument, GameObjectTemplate* p_template);
};
DECOMP_SIZE_ASSERT(GameObjectFactoryEntry, 0x1c);

class GameObjectFactoryRegistry {
public:
	void Register(GameObjectFactoryEntry* p_factory);
	const GameObjectFactoryEntry* Find(const LegoChar* p_className) const;
	GameObject* ConstructByClassName(
		const LegoChar* p_className,
		LegoU32 p_argument,
		GameObjectTemplate* p_template,
		LegoBool32 p_reportUnknownClass
	) const;

	LegoU32 m_unk0x00;
};
DECOMP_SIZE_ASSERT(GameObjectFactoryRegistry, 4);

extern GameObjectFactoryRegistry g_gameObjectFactoryRegistry;

LegoU32 __fastcall GameObjectFactoryReturnZeroForFourArguments(
	GameObjectFactoryEntry*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
);

#endif // LR2_GAMEOBJECTFACTORYREGISTRY_H
