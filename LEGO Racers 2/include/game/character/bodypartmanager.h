#ifndef LR2_BODYPARTMANAGER_H
#define LR2_BODYPARTMANAGER_H

#include "game/controller/gamecontrollerinitialization.h"
#include "game/objects/gamemanager.h"

struct ModelResource;

class BodypartManager : public GameObject {
public:
	BodypartManager(BodypartManagerTemplate* p_template);
	const CharBuffer128* GetHeadModelPath(LegoS32 p_index, LegoU32 p_species);
	LegoS32 GetTorsoTexture(LegoS32 p_index, LegoU32 p_species);
	LegoS32 GetLegTexture(LegoS32 p_index);
	LegoS32 GetFaceTexture(LegoS32 p_group, LegoS32 p_variant);
	ModelResource* GetLoadedHeadModel(LegoS32 p_index);
	LegoS32 FindHeadModel(const LegoChar* p_name, LegoU32* p_species);
	LegoS32 FindTorsoTexture(const LegoChar* p_name, LegoU32* p_species);
	LegoS32 FindLegTexture(const LegoChar* p_name);

	CharBuffer128 m_paths[8];         // 0x58
	LegoS32 m_headModelCount;         // 0x458
	void* m_headModels;               // 0x45c
	LegoS32 m_torsoTextureCount;      // 0x460
	void* m_torsoTextures;            // 0x464
	LegoS32 m_legTextureCount;        // 0x468
	void* m_legTextures;              // 0x46c
	LegoS32 m_faceTextureCount;       // 0x470
	LegoS32 m_faceTextureGroupSize;   // 0x474
	void* m_faceTextures;             // 0x478
	LegoS32 m_alienHeadModelCount;    // 0x47c
	void* m_alienHeadModels;          // 0x480
	LegoS32 m_alienTorsoTextureCount; // 0x484
	void* m_alienTorsoTextures;       // 0x488
	LegoS32 m_ramaHeadModelCount;     // 0x48c
	void* m_ramaHeadModels;           // 0x490
	LegoS32 m_ramaTorsoTextureCount;  // 0x494
	void* m_ramaTorsoTextures;        // 0x498
};

void __cdecl StripBodypartPathAndExtension(LegoChar* p_destination, const LegoChar* p_path);

DECOMP_SIZE_ASSERT(BodypartManager, 0x49c);
DECOMP_STATIC_ASSERT(offsetof(BodypartManager, m_paths) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(BodypartManager, m_headModelCount) == 0x458);
DECOMP_STATIC_ASSERT(offsetof(BodypartManager, m_ramaTorsoTextures) == 0x498);

struct BodypartManagerDispatch {
	GameObjectDispatchView::Destroy m_destroy;
	GameObjectDispatchView::TwoArguments m_applyTransform;
	GameObjectDispatchView::Action m_action0x08;
	GameObjectDispatchView::Action m_action0x0c;
	GameObjectDispatchView::Action m_initialize;
	GameObjectDispatchView::Value m_isActive;
	GameObjectDispatchView::ValueArgument m_action0x18;
	GameObjectDispatchView::Action m_remove;
	GameObjectDispatchView::Value m_value0x20;
	GameObjectDispatchView::Value m_value0x24;
	GameObjectDispatchView::Action m_action0x28;
	GameObjectDispatchView::Action m_action0x2c;
	GameObjectDispatchView::Action m_drawChild;
	GameObjectDispatchView::Update m_action0x34;
	GameObjectDispatchView::Value m_value0x38;
	GameObjectDispatchView::TwoArguments m_action0x3c;
	GameObjectDispatchView::Value m_value0x40;
	GameObjectDispatchView::ValueTwoArguments m_value0x44;
	GameObjectDispatchView::Update m_updateRegion;
	GameObjectDispatchView::Update m_action0x4c;
	GameObjectDispatchView::Value m_objectSize;
};

DECOMP_SIZE_ASSERT(BodypartManagerDispatch, 0x54);

extern const BodypartManagerDispatch g_bodypartManagerDispatch;
GameObject* __fastcall ConstructBodypartManager(
	void* p_storage,
	void* p_unusedEdx,
	BodypartManagerTemplate* p_template
);
LegoU32 __fastcall BodypartManagerGetObjectSize(GameObject* p_object, void* p_unusedEdx);
void __fastcall BodypartManagerInitialize(GameObject* p_object, void* p_unusedEdx);
void __fastcall SetBodypartFaceTextureLoaded(
	BodypartManager* p_manager,
	void* p_unusedEdx,
	LegoU32 p_group,
	LegoU32 p_variant,
	LegoU32 p_load
);

#endif // LR2_BODYPARTMANAGER_H
