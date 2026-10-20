#ifndef LR2_FOYERSCENECIRCLEPOOL_H
#define LR2_FOYERSCENECIRCLEPOOL_H

#include "game/objects/gamemanager.h"

#include <stddef.h>

struct FoyerSceneCirclePool : GameObject {
	FoyerSceneCirclePool();
	void Initialize();
	void Remove();
	LegoU32 UpdateRecords(LegoU32 p_elapsed);
	void DrawRecords();
	void AddObject(void* p_object);
	void RemoveObject(void* p_object);
	void* AllocateRecord();
	void FreeRecord(void* p_record);
	LegoU32 GetAllocationSize() const;

	LegoS32 m_objectCount; // 0x58
	void* m_objects[16];   // 0x5c
	void* m_recordStorage; // 0x9c
	void* m_activeRecords; // 0xa0
	void* m_freeRecords;   // 0xa4
	LegoU32 m_unk0xa8;
};

DECOMP_SIZE_ASSERT(FoyerSceneCirclePool, 0xac);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCirclePool, m_objectCount) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCirclePool, m_recordStorage) == 0x9c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCirclePool, m_freeRecords) == 0xa4);

extern FoyerSceneCirclePool g_foyerSceneCirclePool;

LegoFloat __fastcall GetFoyerSceneRandomUnit(struct GameFloatRandom* p_random, void* p_unusedEdx);
class Vector3* __fastcall GenerateFoyerSceneRandomVector(void* p_randomVector, void* p_unusedEdx, LegoFloat p_range);
LegoU32 __fastcall AddFoyerSceneSecondaryFxCircle(
	void* p_manager,
	void* p_unusedEdx,
	const class Vector3* p_position,
	LegoFloat p_fade,
	LegoU32 p_color
);

#endif // LR2_FOYERSCENECIRCLEPOOL_H
