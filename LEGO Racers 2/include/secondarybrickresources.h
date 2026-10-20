#ifndef LR2_SECONDARYBRICKRESOURCES_H
#define LR2_SECONDARYBRICKRESOURCES_H
#include "charbuffer128.h"
#include "gamemanager.h"
#include "modelresource.h"
struct SecondaryBrickParameters {
	LegoChar m_path[128];
	LegoS32 m_category;
	LegoFloat m_scale;
	LegoS32 m_orientation;
	LegoFloat m_offset;
};
struct SecondaryCarModel {
	ModelResource* m_resource;
	Vector3 m_minimum;
	Vector3 m_maximum;
	Vector3 m_origin;
};
struct SecondaryBrickResource {
	LegoU32 m_loaded;
	CharBuffer128 m_path;
	LegoS32 m_category;
	LegoS32 m_nameId;
	SecondaryCarModel m_models[4];
	LegoFloat m_scale;
	LegoS32 m_orientation;
	LegoFloat m_offset;
	LegoU32 m_worldMask;
};
struct SecondaryCarCollisionStore {
	GameObject* m_owner;
	void* m_collisionData;
	LegoS32 m_count;
	void* m_records[512];
	void* m_payloads[512];
};
struct SecondaryCarResourcePool : GameObject {
	LegoFloat m_unk0x58;
	LegoFloat m_unk0x5c;
	CharBuffer128 m_paths[3];
	void** m_defaultCars;
	LegoS32 m_defaultCarCount;
	void** m_savedCars;
	LegoS32 m_savedCarCount;
	LegoU32 m_loadMode;
	LegoU32 m_createCollisionStore;
	LegoS32 m_entryCount;
	SecondaryBrickResource m_entries[512];
	LegoFloat m_progressStart;
	LegoFloat m_progressEnd;
	SecondaryCarCollisionStore* m_collisionStore;
	LegoU32 m_unk0x27a08;
	LegoS32 m_remainingEntries;
};
DECOMP_SIZE_ASSERT(SecondaryBrickParameters, 0x90);
DECOMP_SIZE_ASSERT(SecondaryCarModel, 0x28);
DECOMP_SIZE_ASSERT(SecondaryBrickResource, 0x13c);
DECOMP_SIZE_ASSERT(SecondaryCarCollisionStore, 0x100c);
DECOMP_SIZE_ASSERT(SecondaryCarResourcePool, 0x27a10);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarResourcePool, m_entries) == 0x1fc);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarResourcePool, m_collisionStore) == 0x27a04);

extern CharBuffer128 g_secondaryGenericBrickPath;
extern CharBuffer128 g_secondaryWorldBrickPaths[5];
const LegoChar* __fastcall GetSecondaryBrickPath(SecondaryCarResourcePool*, void*, LegoS32, LegoS32*);
LegoU32 __fastcall GetSecondaryBrickWorldMask(SecondaryCarResourcePool*, void*, LegoS32);
void __fastcall LoadSecondaryBuilderWorldMasks(
	SecondaryCarResourcePool*,
	void*,
	LegoS32,
	const CharBuffer128*,
	const CharBuffer128*,
	const CharBuffer128*
);
LegoFloat __fastcall GetSecondaryBrickScale(SecondaryCarResourcePool*, void*, LegoS32);
LegoS32 __fastcall GetSecondaryBrickOrientation(SecondaryCarResourcePool*, void*, LegoS32);
LegoFloat __fastcall GetSecondaryBrickOffset(SecondaryCarResourcePool*, void*, LegoS32);
#endif
