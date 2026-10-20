#ifndef LR2_FOYERANIMATIONCONTROLLER_H
#define LR2_FOYERANIMATIONCONTROLLER_H

#include "game/objects/gamemanager.h"
#include "scene/animation/foyeranimationmaster.h"

struct FoyerTemplateInstance;
struct FoyerAnimationController : GameObject {
	FoyerAnimationController(FoyerTemplateInstance*, void*);
	~FoyerAnimationController();
	void Initialize();
	void Remove();
	LegoU32 Update(LegoFloat);
	void LoadTracks();
	void RetainBindings();
	void ReleaseBindings();
	void AdvancePlayers(LegoFloat, LegoU32);
	void ApplyBlendedPose(LegoU32);
	void ApplyPose(LegoU32, const FoyerAnimationPlayer*);
	void BlendPoses(const FoyerAnimationPose*, const FoyerAnimationPose*, LegoFloat, LegoU32);
	void EnsureRunLoop();

	LegoU32 m_unk0x58;
	FoyerTemplateInstance* m_parent;
	FoyerAnimationPose m_pose; // 0x60
	LegoS32 m_skeletonCount;   // 0x80
	LegoU32 m_mode;
	LegoU32 m_defaultAnimationBound;
	LegoFloat m_rate;
	FoyerAnimationPlayer* m_players[4]; // 0x90
	FoyerAnimationMasterCache* m_cache;
	LegoS32 m_trackCount;
	LegoS32 m_tracks[64];   // 0xa8
	void* m_parsedTemplate; // 0x1a8
};
DECOMP_SIZE_ASSERT(FoyerAnimationController, 0x1ac);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationController, m_pose) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationController, m_players) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationController, m_parsedTemplate) == 0x1a8);
struct FoyerAnimationControllerDispatch {
	GameObjectDispatchView m_object;
	GameObjectDispatchView::Value m_getObjectSize;
};
extern const FoyerAnimationControllerDispatch g_foyerAnimationControllerDispatch;
#endif
