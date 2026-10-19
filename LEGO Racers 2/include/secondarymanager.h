#ifndef LR2_SECONDARYMANAGER_H
#define LR2_SECONDARYMANAGER_H

#include "charbuffer128.h"
#include "gamemanager.h"

#include <stddef.h>

struct SecondaryManagerDispatchView {
	GameObjectDispatchView m_gameObject;
	LegoU32(__fastcall* m_getAllocationSize)(GameObject*, void*);
};
DECOMP_SIZE_ASSERT(SecondaryManagerDispatchView, 0x54);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManagerDispatchView, m_getAllocationSize) == 0x50);

void __fastcall ApplySavedAudioLevels(void* p_receiver, void* p_edx);
void __fastcall ApplySavedAudioQuality(void* p_receiver, void* p_edx);

struct SecondaryDisplayMode {
	LegoU32 m_width;
	LegoU32 m_height;
	LegoU32 m_depth;
};
DECOMP_SIZE_ASSERT(SecondaryDisplayMode, 0x0c);

struct SecondaryManager : GameObject {
	LegoU32 m_state; // 0x58
	LegoU32 m_unk0x5c;
	LegoS32 m_requestedState; // 0x60
	LegoU32 m_unk0x64;
	undefined m_unk0x68[0x0c];
	void* m_unk0x74;
	struct SecondaryManagerMediaState* m_unk0x78;
	void* m_unk0x7c;
	void* m_unk0x80;
	void* m_unk0x84;
	void* m_unk0x88;
	void* m_unk0x8c;
	CharBuffer128 m_startupPath; // 0x90
	void* m_unk0x110;
	void* m_resourcePointers[3]; // 0x114
	LegoS32 m_resourceIds[3];    // 0x120
	LegoU32 m_unk0x12c;
	LegoS32 m_unk0x130;
	void* m_unk0x134;
	LegoS32 m_unk0x138;
	void* m_moreResourcePointers[3]; // 0x13c
	LegoS32 m_moreResourceIds[3];    // 0x148
	LegoU32 m_unk0x154[10];          // 0x154
	void* m_unk0x17c;
	void* m_secondaryData[0x2e]; // 0x180
	undefined m_unk0x238[4];
	LegoU32 m_unk0x23c;
	LegoU32 m_unk0x240;
	LegoU32 m_unk0x244;
	LegoU32 m_unk0x248;
	LegoU32 m_unk0x24c;
	LegoU32 m_unk0x250;
	undefined m_unk0x254[4];
	LegoU32 m_unk0x258;
	LegoU32 m_unk0x25c;
	LegoU32 m_unk0x260;
	SecondaryDisplayMode m_displayModes[50]; // 0x264, sorted width/height/depth
	LegoU32 m_unk0x4bc;
	LegoU32 m_unk0x4c0;
	void* m_splashOverlays[2]; // 0x4c4
	LegoU32 m_unk0x4cc;
	LegoU32 m_unk0x4d0;
	LegoU32 m_unk0x4d4[9];

	SecondaryManager(GameObjectTemplate* p_template);
	~SecondaryManager();
	void Initialize();
	void Remove();
	LegoU32 Update(LegoU32 p_frameParameter);
	void ApplySavedAudioLevels() { ::ApplySavedAudioLevels(this, 0); }
	void ApplySavedAudioQuality() { ::ApplySavedAudioQuality(this, 0); }
	LegoBool32 PrepareSplash(LegoU32 p_enable);
	void BeginStartup();
	void EnumerateDisplayModes();
	void DetectKeyboardLanguage();
};

void __fastcall LoadSecondaryManagerAudioResources(SecondaryManager* p_manager, void* p_edx, LegoU32 p_loadMode);
void __fastcall ReleaseSecondaryManagerAudioResources(SecondaryManager* p_manager, void* p_edx);

struct SecondaryManagerMediaSlot {
	LegoU32 m_resourceId;
	LegoU32 m_resourceChangeMask;
	LegoU32 m_state;
	LegoU32 m_stateChangeMask;
};

struct SecondaryManagerMediaState {
	LegoU32 m_enabledSlots;
	SecondaryManagerMediaSlot m_slots[4];
	LegoU32 m_currentSlot;
	void InitializeBindings();
	void EnableSlot(LegoU32 p_index);
	LegoBool32 IsSlotEnabled(LegoU32 p_index);
	LegoBool32 HasActiveSlot();
	SecondaryManagerMediaSlot* GetSlot(LegoU32 p_index);
	void Refresh(LegoU32 p_mask);
};

DECOMP_SIZE_ASSERT(SecondaryManagerMediaSlot, 0x10);
DECOMP_SIZE_ASSERT(SecondaryManagerMediaState, 0x48);

DECOMP_SIZE_ASSERT(SecondaryManager, 0x4f8);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManager, m_state) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManager, m_startupPath) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManager, m_secondaryData) == 0x180);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManager, m_displayModes) == 0x264);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManager, m_unk0x4bc) == 0x4bc);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManager, m_splashOverlays) == 0x4c4);

extern const SecondaryManagerDispatchView g_secondaryManagerDispatch;

#endif // LR2_SECONDARYMANAGER_H
