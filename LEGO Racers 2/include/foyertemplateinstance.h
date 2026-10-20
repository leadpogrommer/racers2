#ifndef LR2_FOYERTEMPLATEINSTANCE_H
#define LR2_FOYERTEMPLATEINSTANCE_H

#include "foyermodelrender.h"
#include "foyerskeleton.h"
#include "gamemanager.h"
#include "physicallifecycle.h"

struct FoyerTemplateInstance : GameObject {
	PhysicalLifecycleState m_physical;   // 0x58
	LegoFloat m_visibilityOverride;      // 0x180
	LegoU32 m_anyView;                   // 0x184
	void* m_view;                        // 0x188
	LegoU32 m_state;                     // 0x18c
	LegoS32 m_skeletonCount;             // 0x190
	FoyerSkeletonRecord* m_skeletons;    // 0x194
	LegoS32 m_attachmentSkeletonIndex;   // 0x198
	Vector3 m_lookTarget;                // 0x19c
	Vector3 m_lookUp;                    // 0x1a8
	LegoU32 m_lookEnabled;               // 0x1b4
	LegoFloat m_lookThreshold;           // 0x1b8
	Quaternion m_lookOrientation;        // 0x1bc
	LegoFloat m_lookBlend;               // 0x1cc
	LegoU32 m_lookFlags;                 // 0x1d0
	Quaternion m_appliedLookOrientation; // 0x1d4
	GameObject* m_animationChild;        // 0x1e4
	LegoU32 m_unk0x1e8;
	Vector3 m_skeletonOffset;        // 0x1ec
	Vector3 m_localVelocity;         // 0x1f8
	LegoU32 m_templatePayload[0x20]; // 0x204
	void* m_attachment;              // 0x284
	Vector3 m_impulse;               // 0x288
	LegoU32 m_movementInitialized;   // 0x294
	Vector3 m_movementStart;         // 0x298

	FoyerTemplateInstance(GameObjectTemplate*);
	~FoyerTemplateInstance();
	void Initialize();
	void Remove();
};
DECOMP_SIZE_ASSERT(FoyerTemplateInstance, 0x2a4);
DECOMP_STATIC_ASSERT(offsetof(FoyerTemplateInstance, m_physical) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(FoyerTemplateInstance, m_skeletonCount) == 0x190);
DECOMP_STATIC_ASSERT(offsetof(FoyerTemplateInstance, m_templatePayload) == 0x204);
DECOMP_STATIC_ASSERT(offsetof(FoyerTemplateInstance, m_attachment) == 0x284);
DECOMP_STATIC_ASSERT(offsetof(FoyerTemplateInstance, m_movementStart) == 0x298);

struct FoyerTemplateInstanceDispatch {
	GameObjectDispatchView m_object;
	GameObjectDispatchView::Value m_getObjectSize;
};
DECOMP_SIZE_ASSERT(FoyerTemplateInstanceDispatch, 0x54);
extern const FoyerTemplateInstanceDispatch g_foyerTemplateInstanceDispatch;
extern const PhysicalLifecycleDispatch g_foyerTemplateLifecycleDispatch;
void __fastcall InitializeFoyerTemplateInstance(GameObject*, void*);
void __fastcall RemoveFoyerTemplateInstance(GameObject*, void*);
LegoU32 __fastcall UpdateFoyerTemplateInstance(GameObject*, void*, LegoFloat);
void __fastcall UpdateFoyerTemplateAttachment(GameObject*, void*);
LegoU32 __fastcall IntegrateFoyerTemplateMotion(GameObject*, void*);
LegoU32 __fastcall ApplyFoyerTemplateForces(GameObject*, void*);
void __fastcall UpdateFoyerTemplateLookOrientation(GameObject*, void*, LegoFloat);
LegoU32 __fastcall GetFoyerTemplateObjectCategory(GameObject*, void*);
LegoU32 __fastcall GetFoyerTemplateInstanceObjectSize(GameObject*, void*);

#endif
