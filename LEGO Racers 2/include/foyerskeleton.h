#ifndef LR2_FOYERSKELETON_H
#define LR2_FOYERSKELETON_H

#include "gamemanager.h"
#include "quaternion.h"
#include "vector3.h"

struct FoyerSkeletonRecord {
	GameObject* m_owner;           // 0x00
	FoyerSkeletonRecord* m_parent; // 0x04
	LegoS32 m_index;               // 0x08
	LegoS32 m_parentIndex;         // 0x0c
	Vector3 m_localPosition;       // 0x10
	Quaternion m_localOrientation; // 0x1c
	Quaternion m_worldOrientation; // 0x2c
	Vector3 m_worldPosition;       // 0x3c
};
DECOMP_SIZE_ASSERT(FoyerSkeletonRecord, 0x48);
DECOMP_STATIC_ASSERT(offsetof(FoyerSkeletonRecord, m_worldOrientation) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSkeletonRecord, m_worldPosition) == 0x3c);

struct FoyerSkeletonDefinition {
	LegoS16 m_index;
	LegoS16 m_parentIndex;
	Quaternion m_orientation;
	Vector3 m_position;
};
DECOMP_SIZE_ASSERT(FoyerSkeletonDefinition, 0x20);

FoyerSkeletonRecord* __fastcall ConstructFoyerSkeletonRecord(FoyerSkeletonRecord*, void*);
void __fastcall BindFoyerSkeletonRecord(FoyerSkeletonRecord*, void*, const FoyerSkeletonDefinition*, GameObject*);
void __fastcall RebuildFoyerSkeletonTransforms(GameObject*, void*);

void __fastcall RebuildFoyerSkeletonRecordTransform(FoyerSkeletonRecord*, void*);

#endif
