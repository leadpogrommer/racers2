#ifndef LR2_CAR_DISPLAY_H
#define LR2_CAR_DISPLAY_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct GameObject;
struct GameObjectTemplate;
struct CarDisplayTransformFunctionTable;
extern const CarDisplayTransformFunctionTable g_carDisplayBaseTransformDispatch;
void __fastcall CarDisplayLifecycleCopyTargetData(void*, void*, LegoU32*, LegoU32, LegoU32);
void* __fastcall DestroyCarDisplayBaseTransform(void*, void*, LegoU32);
void __fastcall CarDisplayModelRemove(GameObject*, void*);

GameObject* __fastcall ConstructCarDisplayIndexedRecordOwner(void*, void*, LegoU32, GameObjectTemplate*);

// Observed storage used by the menu preview. The nine groups each hold four
// renderer objects; the first 0x10 bytes hold the name-array pointer and state.
struct CarDisplay {
	void** m_nameObjects;     // +0x00, 36 pointers
	void* m_resourceTable;    // +0x04, SecondaryManager+0x74
	LegoS32 m_activeCount;    // +0x08
	LegoS32 m_selectedChoice; // +0x0c
	void* m_parts[36];        // +0x10
};
DECOMP_SIZE_ASSERT(CarDisplay, 0xa0);
DECOMP_STATIC_ASSERT(offsetof(CarDisplay, m_parts) == 0x10);

void InitializeCarDisplay(CarDisplay* p_display);
void InitializeCarDisplayModelPaths();
void ReleaseCarDisplay(CarDisplay* p_display);
void ResetCarDisplay(CarDisplay* p_display);
void __fastcall UpdateCarDisplayChoice(
	CarDisplay* p_display,
	void* p_unusedEdx,
	const LegoU32* p_choiceRecord,
	const LegoFloat* p_vehicleData,
	const LegoFloat* p_rotation,
	LegoU32 p_mode
);
void __fastcall ApplyCarDisplayChoice(
	CarDisplay* p_display,
	void* p_unusedEdx,
	const LegoU32* p_choiceRecord,
	const LegoFloat* p_vehicleData,
	const LegoFloat* p_rotation,
	LegoU32 p_mode
);

GameObject* CreateCarDisplayNameObject();
GameObject* CreateCarDisplayPartForModel(const LegoChar*, LegoU32);
void DestroyCarDisplayModelObject(GameObject*);
#endif
