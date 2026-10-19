#ifndef LR2_REVERBMANAGER_H
#define LR2_REVERBMANAGER_H

#include "gamemanager.h"
#include "reverbgeometry.h"

struct ReverbManager : GameObject {
	LegoS32 m_volumeCount;                  // 0x58
	ReverbVolumeView* m_volumes[8];         // 0x5c
	LegoFloat m_defaultFirstParameter;      // 0x7c
	LegoFloat m_defaultSecondParameter;     // 0x80
	LegoU32 m_defaultState;                 // 0x84, integer one
	LegoU32 m_currentState;                 // 0x88
	ReverbVolumeView* m_selectedVolumes[2]; // 0x8c
	Vector3 m_lastPositions[2];             // 0x94
	GameObject* m_cameras[2];               // 0xac
	LegoU32 m_dirty;                        // 0xb4
	LegoS32 m_countdown;                    // 0xb8

	void Reset();
	void Configure(LegoU32, LegoFloat, LegoFloat);
	LegoBool32 CameraHasNoVolume(GameObject*) const;
	LegoBool32 HasCameraMoved(LegoS32, LegoFloat, LegoFloat, LegoFloat) const;
	LegoU32 Update(LegoU32);
	void SelectCameraVolume(LegoS32, ReverbVolumeView*);
	void ResetCameraVolume(LegoS32, LegoU32);
	void DisableOutput();
	void RestoreOutput();
	void RecomputeCameraVolume(LegoS32, const Vector3*);
	ReverbVolumeView* FindVolume(const Vector3*) const;
};
DECOMP_SIZE_ASSERT(ReverbManager, 0xbc);
DECOMP_STATIC_ASSERT(offsetof(ReverbManager, m_volumes) == 0x5c);
DECOMP_STATIC_ASSERT(offsetof(ReverbManager, m_defaultFirstParameter) == 0x7c);
DECOMP_STATIC_ASSERT(offsetof(ReverbManager, m_selectedVolumes) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(ReverbManager, m_lastPositions) == 0x94);
DECOMP_STATIC_ASSERT(offsetof(ReverbManager, m_cameras) == 0xac);
DECOMP_STATIC_ASSERT(offsetof(ReverbManager, m_dirty) == 0xb4);

extern LegoU32 g_reverbDefaultState;
extern const GameObjectDispatchView g_reverbManagerDispatch;
GameObject* __fastcall ConstructReverbManager(void*, void*, GameObjectTemplate*);
void __fastcall MarkReverbStateDirty(GameObject*, void*);

#endif
