#include "foyerscenecirclepool.h"

#include "gamecontroller.h"
#include "gameobjectdispatch.h"
#include "gamerandom.h"
#include "memoryheap.h"
#include "renderer.h"
#include "rendererdispatch.h"
#include "vector3.h"

#include <stdlib.h>
#include <string.h>

struct FoyerSceneCircleRecord {
	LegoFloat m_randomVectorX;
	LegoFloat m_randomVectorY;
	LegoFloat m_randomVectorZ;
	LegoU32 m_unk0x0c;
	Vector3 m_position;
	LegoU32 m_unk0x1c;
	LegoU32 m_unk0x20[5];
	LegoFloat m_unk0x34;
	LegoFloat m_random0;
	LegoFloat m_random1;
	LegoFloat m_random2;
	LegoU32 m_unk0x44;
	LegoU32 m_state;
	LegoFloat m_randomOffset;
	LegoU32 m_unk0x50;
	LegoU32 m_unk0x54;
	LegoU32 m_unk0x58;
	LegoU32 m_unk0x5c;
	FoyerSceneCircleRecord* m_previous;
	FoyerSceneCircleRecord* m_next;
};
DECOMP_SIZE_ASSERT(FoyerSceneCircleRecord, 0x68);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCircleRecord, m_position) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCircleRecord, m_state) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCircleRecord, m_previous) == 0x60);

struct FoyerSceneCircleManagerView {
	LegoU8 m_opaque0x00[4];
	FoyerSceneCircleRecord* m_records[0x100];
	LegoU32 m_count;
	LegoU8 m_opaque0x408[4];
};
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneCircleManagerView, m_count) == 0x404);

struct FoyerSceneRandomVectorView {
	LegoU8 m_opaque0x00[0x24];
	Vector3 m_value;
};

struct FoyerSceneCirclePoolDispatch {
	GameObjectDispatchView m_base;
	LegoU32(__fastcall* m_getAllocationSize)(GameObject*, void*);
};

void __fastcall CirclePoolInitialize(GameObject* p_object, void* p_edx);
LegoU32 __fastcall CirclePoolUpdate(GameObject* p_object, void* p_edx, LegoU32 p_elapsed);
void __fastcall CirclePoolRemove(GameObject* p_object, void* p_edx);
void __fastcall CirclePoolDraw(GameObject* p_object, void* p_edx);
LegoU32 __fastcall CirclePoolGetAllocationSize(GameObject* p_object, void* p_edx);

extern const FoyerSceneCirclePoolDispatch g_foyerSceneCirclePoolDispatch;

// Retail dispatch table at 0x0053632c (21 slots).
const FoyerSceneCirclePoolDispatch g_foyerSceneCirclePoolDispatch = {
	{
		&GameObjectBaseDestroy,
		&GameObjectBaseApplyChildTransform,
		&GameObjectBaseNoOp,
		&GameObjectBaseNoOp,
		&CirclePoolInitialize,
		&GameObjectBaseZero,
		&CirclePoolUpdate,
		&CirclePoolRemove,
		&GameObjectBaseZero,
		&GameObjectBaseZero,
		&GameObjectBaseNoOp,
		&GameObjectBaseNoOp,
		&CirclePoolDraw,
		&GameObjectBaseNoOpOneArgument,
		&GameObjectBaseZero,
		&GameObjectBaseNoOpTwoArguments,
		&GameObjectBaseZero,
		&GameObjectBaseZeroTwoArguments,
		&GameObjectBaseUpdateRegion,
		&GameObjectBaseNoOpOneArgument,
	},
	&CirclePoolGetAllocationSize,
};

LegoFloat NextCirclePoolRandom(GameFloatRandom* p_random)
{
	p_random->m_frame = p_random->m_multiplier * p_random->m_frame + p_random->m_increment;
	LegoU32 bits = (p_random->m_mantissaMask & p_random->m_frame) | p_random->m_state;
	return *(LegoFloat*) &bits;
}

// STUB: LEGORACERS2 0x0043bcb0
LegoFloat __fastcall GetFoyerSceneRandomUnit(GameFloatRandom* p_random, void*)
{
	return NextCirclePoolRandom(p_random) - 1.0f;
}

// STUB: LEGORACERS2 0x00470430
LegoU32 __fastcall AddFoyerSceneSecondaryFxCircle(
	void* p_manager,
	void*,
	const Vector3* p_position,
	LegoFloat p_fade,
	LegoU32 p_color
)
{
	FoyerSceneCircleManagerView* manager = (FoyerSceneCircleManagerView*) p_manager;
	if (manager->m_count == 0x100) {
		return 0;
	}

	FoyerSceneCircleRecord* record = (FoyerSceneCircleRecord*) g_foyerSceneCirclePool.AllocateRecord();
	if (record == NULL) {
		return 0;
	}
	manager->m_records[manager->m_count++] = record;

	GameRandomState* randomState = GetGameRandomState();
	GameFloatRandom* random = &randomState->m_floatStreams[1];
	GameFloatRandom* secondaryRandom = &randomState->m_floatStreams[1];
	record->m_unk0x0c = p_color;
	record->m_unk0x1c = p_color;
	record->m_state = 2;
	record->m_position = *p_position;
	record->m_unk0x34 = 0.3333333432674408f;
	LegoFloat randomBase = GetFoyerSceneRandomUnit(random, NULL) * 0.5f + 0.5f;
	record->m_random0 = randomBase;
	record->m_random1 = randomBase;
	record->m_unk0x20[0] = *(LegoU32*) ((LegoU8*) p_manager + 0x408);
	record->m_unk0x20[1] = 0;
	record->m_unk0x20[2] = 0x3f800000;
	record->m_unk0x20[3] = 0x3f800000;
	record->m_unk0x20[4] = 0;
	void* randomVectorObject = *(void**) ((LegoU8*) (size_t) g_gameController.m_unk0xa4 + 0x5c);
	GenerateFoyerSceneRandomVector(randomVectorObject, NULL, 3.0f);
	Vector3* randomOffset = (Vector3*) ((LegoU8*) randomVectorObject + 0x24);
	record->m_randomVectorX = randomOffset->m_x;
	record->m_randomVectorY = randomOffset->m_y + 0.01f;
	record->m_randomVectorZ = randomOffset->m_z;

	if (p_fade == 0.0f) {
		LegoFloat integerRandom = GetFoyerSceneRandomUnit(secondaryRandom, NULL);
		record->m_unk0x50 = 0x40400000;
		record->m_unk0x54 = 2;
		record->m_randomOffset = integerRandom * 4.0f + 2.0f;
	}
	else {
		LegoFloat halfFade = p_fade * 0.5f;
		record->m_unk0x50 = *(LegoU32*) &p_fade;
		record->m_unk0x54 = 2;
		record->m_randomOffset = GetFoyerSceneRandomUnit(secondaryRandom, NULL) * halfFade + halfFade;
	}
	return 1;
}

// STUB: LEGORACERS2 0x0047b400
Vector3* __fastcall GenerateFoyerSceneRandomVector(void* p_randomVector, void*, LegoFloat p_range)
{
	FoyerSceneRandomVectorView* randomVector = (FoyerSceneRandomVectorView*) p_randomVector;
	GameFloatRandom* random = &GetGameRandomState()->m_floatStreams[1];
	LegoFloat radius = p_range * 0.003f;
	LegoFloat x = GetFoyerSceneRandomUnit(random, NULL) * radius;
	LegoFloat y = GetFoyerSceneRandomUnit(random, NULL) * radius;
	LegoFloat z = GetFoyerSceneRandomUnit(random, NULL) * radius;
	randomVector->m_value.m_x = x + x - radius;
	randomVector->m_value.m_y = y + y - radius;
	randomVector->m_value.m_z = z + z - radius;
	return &randomVector->m_value;
}

// STUB: LEGORACERS2 0x004bd280
FoyerSceneCirclePool::FoyerSceneCirclePool() : GameObject(0x700, NULL, NULL), m_objectCount(0), m_recordStorage(NULL)
{
	m_vtable = &g_foyerSceneCirclePoolDispatch.m_base;
}

// STUB: LEGORACERS2 0x004bd2e0
void FoyerSceneCirclePool::Initialize()
{
	GameObject::Initialize();
	m_recordStorage = AllocateHeapMemory(0x65900);
	m_activeRecords = NULL;
	m_freeRecords = m_recordStorage;
	LegoU8* storage = (LegoU8*) m_recordStorage;
	LegoU32 i;
	for (i = 0; i < 4000; ++i) {
		LegoU8* record = storage + i * 0x68;
		*(LegoU32*) (record + 0x20) = 0;
		*(LegoU32*) (record + 0x60) = 0;
		*(LegoU32*) (record + 0x64) = 0;
	}
	for (i = 0; i < 4000; ++i) {
		FoyerSceneCircleRecord* record = (FoyerSceneCircleRecord*) (storage + i * 0x68);
		record->m_previous = i == 0 ? NULL : (FoyerSceneCircleRecord*) (storage + (i - 1) * 0x68);
		record->m_next = i == 3999 ? NULL : (FoyerSceneCircleRecord*) (storage + (i + 1) * 0x68);
	}
}

// FUNCTION: LEGORACERS2 0x004bd380
void FoyerSceneCirclePool::Remove()
{
	if (m_recordStorage != NULL) {
		FreeHeapMemory(m_recordStorage);
		m_recordStorage = NULL;
	}
	GameObject::Remove();
}

// FUNCTION: LEGORACERS2 0x004bd3b0
LegoU32 FoyerSceneCirclePool::GetAllocationSize() const
{
	return 0xac;
}

// STUB: LEGORACERS2 0x004bd3c0
LegoU32 FoyerSceneCirclePool::UpdateRecords(LegoU32)
{
	for (LegoS32 i = 0; i < m_objectCount; ++i) {
		GameObject* object = (GameObject*) m_objects[i];
		((void(__fastcall*)(GameObject*, void*)) object->m_vtable->m_action0x04)(object, NULL);
	}
	return 1;
}

// STUB: LEGORACERS2 0x004bd3f0
void FoyerSceneCirclePool::DrawRecords()
{
	DisplayRenderer* renderer = g_displayRenderer;
	RendererDispatchView* dispatch = (RendererDispatchView*) renderer->m_vtable;
	dispatch->m_action0x124(renderer, NULL, 0);
	*(LegoU32*) ((LegoU8*) renderer + 0x4c8) = 0;
	*(LegoU32*) ((LegoU8*) renderer + 0x4cc) = 0;
	void* rendererState = *(void**) ((LegoU8*) renderer + 0x80);
	void** rendererStateTable = *(void***) rendererState;
	((void(__fastcall*)(void*, void*)) rendererStateTable[1])(rendererState, NULL);
	if (*(LegoU32*) ((LegoU8*) renderer + 0x4c8) != 0) {
		dispatch->m_action0x124(renderer, NULL, 0xe);
	}
	dispatch->m_action0x124(renderer, NULL, 5);
	for (LegoS32 i = 0; i < m_objectCount; ++i) {
		GameObject* object = (GameObject*) m_objects[i];
		((void(__fastcall*)(GameObject*, void*)) object->m_vtable->m_action0x08)(object, NULL);
	}
	dispatch->m_action0x124(renderer, NULL, 6);
}

// FUNCTION: LEGORACERS2 0x004bd480
void FoyerSceneCirclePool::AddObject(void* p_object)
{
	m_objects[m_objectCount++] = p_object;
}

// FUNCTION: LEGORACERS2 0x004bd4a0
void FoyerSceneCirclePool::RemoveObject(void* p_object)
{
	for (LegoS32 i = 0; i < m_objectCount; ++i) {
		if (m_objects[i] == p_object) {
			--m_objectCount;
			for (; i < m_objectCount; ++i) {
				m_objects[i] = m_objects[i + 1];
			}
			return;
		}
	}
}

// STUB: LEGORACERS2 0x004bd4f0
void* FoyerSceneCirclePool::AllocateRecord()
{
	FoyerSceneCircleRecord* record = (FoyerSceneCircleRecord*) m_freeRecords;
	if (record == NULL) {
		return NULL;
	}
	m_freeRecords = record->m_next;
	record->m_next = (FoyerSceneCircleRecord*) m_activeRecords;
	record->m_previous = NULL;
	if (m_activeRecords != NULL) {
		((FoyerSceneCircleRecord*) m_activeRecords)->m_previous = record;
	}
	m_activeRecords = record;
	record->m_unk0x1c = 0xffffffff;
	record->m_unk0x0c = 0xffffffff;
	record->m_random0 = 1.0f;
	record->m_random1 = 1.0f;
	record->m_unk0x34 = 1.0f;
	record->m_state = 0;
	record->m_randomVectorX = 0.0f;
	record->m_unk0x44 = 0;
	record->m_random2 = 0.0f;
	*(LegoU32*) ((LegoU8*) record + 0x04) = 0;
	*(LegoU32*) ((LegoU8*) record + 0x08) = 0;
	return record;
}

// FUNCTION: LEGORACERS2 0x004bd590
void FoyerSceneCirclePool::FreeRecord(void* p_value)
{
	if (p_value == NULL) {
		return;
	}
	FoyerSceneCircleRecord* p_record = (FoyerSceneCircleRecord*) p_value;
	FoyerSceneCircleRecord* next = p_record->m_next;
	FoyerSceneCircleRecord* previous = p_record->m_previous;
	if (next != NULL) {
		next->m_previous = previous;
	}
	if (previous != NULL) {
		previous->m_next = next;
	}
	else {
		m_activeRecords = next;
	}
	p_record->m_previous = NULL;
	p_record->m_next = (FoyerSceneCircleRecord*) m_freeRecords;
	m_freeRecords = p_record;
}

void __fastcall CirclePoolInitialize(GameObject* p_object, void*)
{
	((FoyerSceneCirclePool*) p_object)->Initialize();
}
LegoU32 __fastcall CirclePoolUpdate(GameObject* p_object, void*, LegoU32 p_elapsed)
{
	return ((FoyerSceneCirclePool*) p_object)->UpdateRecords(p_elapsed);
}
void __fastcall CirclePoolRemove(GameObject* p_object, void*)
{
	((FoyerSceneCirclePool*) p_object)->Remove();
}
void __fastcall CirclePoolDraw(GameObject* p_object, void*)
{
	((FoyerSceneCirclePool*) p_object)->DrawRecords();
}
LegoU32 __fastcall CirclePoolGetAllocationSize(GameObject* p_object, void*)
{
	return ((FoyerSceneCirclePool*) p_object)->GetAllocationSize();
}

// GLOBAL: LEGORACERS2 0x0067a318
FoyerSceneCirclePool g_foyerSceneCirclePool;
