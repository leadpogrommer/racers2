#ifndef LR2_GAMECONTROLLERINITIALIZATION_H
#define LR2_GAMECONTROLLERINITIALIZATION_H

#include "charbuffer128.h"
#include "gamecontroller.h"

// Interfaces for concrete startup dependencies. Their bodies and dispatch
// tables must be recovered before gamecontrollerinitialization.cpp is linked.
// These declarations supply no fallback implementation.
LegoBool32 IsRunningFromOpticalMedia();                                           // 42af30
void __fastcall InitializeGameControllerDefaults(GameController*, void*);         // 40a970
void __fastcall ResetGameControllerSession(GameController*, void*);               // 40abc0
void InitializeGameControllerPlayers();                                           // 40ac30
void InitializeLocalizedPlayerNames();                                            // 40a3d0
void __fastcall ApplyControllerFonts(GameController*, void*);                     // 40a230
void __fastcall LoadControllerSceneResources(GameController*, void*, LegoU32);    // 40a170
void __fastcall ReleaseControllerSceneResources(GameController*, void*, LegoU32); // 40a1d0
void __fastcall InitializeControllerBindingProfile(
	GameController*,
	void*,
	GameControllerPairGroups*,
	LegoU32 p_enabled,
	LegoU32 p_index
);
void __fastcall StartControllerMusic(GameController*, void*, LegoS32 p_mode, LegoS32 p_track);

struct BodypartManagerTemplate {
	GameObjectTemplate m_base;
	CharBuffer128 m_paths[8];
};
DECOMP_SIZE_ASSERT(BodypartManagerTemplate, 0x434);

GameObject* __fastcall ConstructBodypartManager(void*, void*, BodypartManagerTemplate*); // inlined407250
GameObject* __fastcall ConstructBattleManager(void*, void*, GameObjectTemplate*);        // 406040
GameObject* __fastcall ConstructFoyerManager(void*, void*, GameObjectTemplate*);         // 43a2f0
GameObject* __fastcall ConstructTextBoxManager(void*, void*, GameObjectTemplate*);       // 438e50
GameObject* __fastcall ConstructNpcTextManager(void*, void*, GameObjectTemplate*);       // 45f9e0
GameObject* __fastcall ConstructSecondaryFxManager(void*, void*, GameObjectTemplate*);   // 478f30
GameObject* __fastcall ConstructNetworkAppLayer(void*, void*, GameObjectTemplate*);      // 4935b0
GameObject* __fastcall ConstructKillSphereSupervisor(void*, void*, GameObjectTemplate*); // 4b5460
GameObject* __fastcall ConstructReverbManager(void*, void*, GameObjectTemplate*);        // 43d720
GameObject* __fastcall ConstructScreenPrinter(void*, void*, GameObjectTemplate*);        // 45f770
GameObject* __fastcall ConstructPauseDetector(void*, void*, GameObjectTemplate*);        // 407f00
GameObject* __fastcall ConstructControllerStartupScene(void*, void*);                    // 406ab0
void* __fastcall ConstructControllerEventRecords(void*, void*);                          // 434240
void* __fastcall ConstructControllerSaveRecords(void*, void*);                           // 410fc0
void* __fastcall ConstructControllerSelectionState(void*, void*);                        // 4338c0
void* ConstructGamePauseObserver(void*);                                                 // inline407250, table52f5d8
void* ConstructControllerAuxiliaryState(void*);                                          // inline407250, table52f57c
void RegisterGameControllerCallbacks();                                                  // 45f610

extern CharBuffer128 g_gameLocalizationPath;
extern const LegoChar g_gameLanguageSuffixes[9][4];
#endif
