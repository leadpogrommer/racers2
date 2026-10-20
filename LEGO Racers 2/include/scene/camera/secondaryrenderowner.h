#ifndef LR2_SECONDARYRENDEROWNER_H
#define LR2_SECONDARYRENDEROWNER_H
#include "physics/dynamics/physicallifecycle.h"

struct SecondaryRenderOwnerTemplate {
	GameObjectTemplate m_base;
	LegoS32 m_modelIds[4];
	Quaternion m_orientation;
	Vector3 m_position;
	LegoU32 m_flags;
	LegoFloat m_nearDistance;
};
DECOMP_SIZE_ASSERT(SecondaryRenderOwnerTemplate, 0x68);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwnerTemplate, m_orientation) == 0x44);

struct SecondaryRenderOwner {
	GameObject m_object;
	PhysicalLifecycleState m_physical;
	GameObject* m_target;       // 0x180
	GameObject* m_secondTarget; // 0x184
	Vector3 m_position;         // 0x188
	Quaternion m_orientation;   // 0x194
	LegoFloat m_fieldOfView;
	LegoFloat m_nearDistance;
	LegoFloat m_farDistance;
	LegoS32 m_viewportLeft;
	LegoS32 m_viewportTop;
	LegoS32 m_viewportWidth;
	LegoS32 m_viewportHeight;
	LegoU32 m_geometryUsable;
	LegoU32 m_viewportIndex;
	LegoS32 m_shadowState;
	LegoU32 m_updateProjectionRatio;
	LegoFloat m_horizontalFov;
	LegoFloat m_verticalFov;
	Vector3 m_planeNormals[6]; // 0x1d8
	Vector3 m_planePoints[6];  // 0x220
	Vector3* m_planePointPointer;
	LegoU32 m_planeCount;
	Vector3 m_worldPlaneNormals[6]; // 0x270
	Vector3 m_worldPlanePoints[6];  // 0x2b8
	Vector3* m_worldPlanePointPointer;
	LegoU32 m_worldPlaneCount;
	Vector3 m_forward; // 0x308
	LegoU32 m_projectionMode;
	LegoU32 m_alternateProjectionScale;
	LegoFloat m_orthographicScale;
	LegoFloat m_movementSpeed;
	Vector3 m_angles;
	Vector3 m_offset;       // 0x330
	Vector3 m_secondOffset; // 0x33c
	LegoU32 m_settleFrames;
	LegoU32 m_unk0x34c;
};
DECOMP_SIZE_ASSERT(SecondaryRenderOwner, 0x350);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwner, m_physical) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwner, m_target) == 0x180);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwner, m_position) == 0x188);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwner, m_planePointPointer) == 0x268);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwner, m_worldPlanePointPointer) == 0x300);
DECOMP_STATIC_ASSERT(offsetof(SecondaryRenderOwner, m_offset) == 0x330);
GameObject* __fastcall ConstructSecondaryRenderOwner(GameObject*, void*, const SecondaryRenderOwnerTemplate*);
void __fastcall ReleaseSecondaryRenderOwner(GameObject*, void*);
#endif
