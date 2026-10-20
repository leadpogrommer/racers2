#ifndef LR2_CONTROLLERLOADINGPROGRESS_H
#define LR2_CONTROLLERLOADINGPROGRESS_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct GlobalOverlaySprite;
struct ControllerLoadingProgress {
	LegoU32 m_active;
	LegoS32 m_mode;
	LegoFloat m_progress;
	LegoFloat m_elapsed;
	LegoFloat m_lastUpdate;
	LegoFloat m_targets[5];
	LegoFloat m_values[5];
	LegoFloat m_velocities[5];
	GlobalOverlaySprite* m_sprites[18];
};
DECOMP_SIZE_ASSERT(ControllerLoadingProgress, 0x98);
DECOMP_STATIC_ASSERT(offsetof(ControllerLoadingProgress, m_targets) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(ControllerLoadingProgress, m_values) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(ControllerLoadingProgress, m_velocities) == 0x3c);
DECOMP_STATIC_ASSERT(offsetof(ControllerLoadingProgress, m_sprites) == 0x50);

void __fastcall InitializeControllerSelectionProgress(void*, void*, LegoU32);
void __fastcall FinalizeControllerSelectionProgress(void*, void*);
void __fastcall UpdateControllerSelectionProgress(void*, void*, LegoU32);
void __fastcall SetControllerSelectionProgressValue(void*, void*, LegoFloat);
void __fastcall AdvanceControllerLoadingEqualizer(ControllerLoadingProgress*, void*, LegoFloat);
void __fastcall CreateControllerLoadingSprites(ControllerLoadingProgress*, void*);
void __fastcall UpdateControllerLoadingSprites(ControllerLoadingProgress*, void*);
void __fastcall UpdateControllerLoadingEqualizerBar(ControllerLoadingProgress*, void*, LegoS32, LegoFloat);
void __fastcall LoadControllerLoadingTextures(void*, void*, LegoS32);
void __fastcall ReleaseControllerLoadingTextures(void*, void*, LegoS32);

struct ControllerLoadingTuning {
	LegoFloat m_wordSize[2];
	LegoFloat m_progressSize[2];
	LegoFloat m_equalizerSize[2];
	LegoFloat m_phasePeriod;
	undefined m_unk0x1c[4];
	LegoFloat m_equalizerOrigin[2];
	LegoFloat m_progressOrigin[2];
};
DECOMP_SIZE_ASSERT(ControllerLoadingTuning, 0x30);
extern ControllerLoadingTuning g_controllerLoadingTuning;

#endif
