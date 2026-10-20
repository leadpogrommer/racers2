#ifndef LR2_GAMECONTROLLER_H
#define LR2_GAMECONTROLLER_H

#include "game/objects/gamemanager.h"

// Working names for the embedded records; their complete gameplay roles are
// still being traced. Fields not written by a retail constructor stay untouched.
struct GameControllerName {
	GameControllerName* Initialize();
	GameControllerName* SetNarrowText(const LegoChar* p_text);
	GameControllerName* SetWideText(const LegoU16* p_text);
	LegoU16 m_text[9];
	undefined m_unk0x12[2];
};
DECOMP_SIZE_ASSERT(GameControllerName, 0x14);

struct GameControllerScore {
	GameControllerScore* Initialize();
	GameControllerName m_name;
	LegoFloat m_value;
};
DECOMP_SIZE_ASSERT(GameControllerScore, 0x18);

struct GameControllerPair {
	LegoU32 m_first;
	LegoU32 m_second;
};
struct GameControllerPairGroups {
	void Initialize();
	LegoU32 m_index;
	LegoU32 m_enabled;
	LegoU32 m_unk0x08;
	GameControllerPair m_pairs[2];
	GameControllerPair m_firstGroups[9];
	GameControllerPair m_secondGroups[9];
};
DECOMP_SIZE_ASSERT(GameControllerPairGroups, 0xac);

struct GameControllerSelection {
	void Initialize();
	void UnlockWorld(LegoU32 p_world);
	void UnlockTrack(LegoU32 p_world, LegoU32 p_track);
	LegoBool32 IsTrackUnlocked(LegoU32 p_world, LegoU32 p_track) const;
	void UnlockReward(LegoU32 p_world, LegoU32 p_reward, LegoU32 p_includeBonus);
	LegoBool32 HasReward(LegoU32 p_world, LegoU32 p_reward) const;
	void UnlockDefaults(LegoU32 p_includeFinalTrack, LegoU32 p_rewards);
	LegoS32 m_values[8];
};
DECOMP_SIZE_ASSERT(GameControllerSelection, 0x20);

struct GameControllerItem {
	GameControllerItem* Initialize();
	undefined m_data[0x14];
};
struct GameControllerItemBlock {
	GameControllerItem m_items[36];
	undefined m_unk0x2d0[0x2f4 - 0x2d0];
};
DECOMP_SIZE_ASSERT(GameControllerItemBlock, 0x2f4);

struct GameControllerPlayerBlock {
	GameControllerItem m_items[36];
	undefined m_unk0x2d0[0x2e0 - 0x2d0];
	LegoU32 m_state[4];
	undefined m_unk0x2f0[0x344 - 0x2f0];
};
DECOMP_SIZE_ASSERT(GameControllerPlayerBlock, 0x344);

struct LocalizedStringTable;
class BitmapFontSet;
class InputController;

struct GameController : GameObject {
	GameController(GameObjectTemplate* p_template);
	~GameController();
	void Initialize();
	void Remove();
	void PlayStartupMovies();
	LegoU32 Update(LegoU32 p_elapsed);

	LegoU32 m_state;          // 0x58
	LegoU32 m_stateStarted;   // 0x5c
	LegoS32 m_requestedState; // 0x60
	LegoU32 m_startupReady;   // 0x64
	LegoU32 m_unk0x68;
	GameObject* m_pauseDetector; // 0x6c
	void* m_unk0x70;
	LocalizedStringTable* m_localizedStrings; // 0x74
	GameObject* m_secondaryManager;           // 0x78
	GameObject* m_raceManager;                // 0x7c
	GameObject* m_battleManager;              // 0x80
	GameObject* m_foyerManager;               // 0x84
	void* m_unk0x88;
	void* m_unk0x8c;
	void* m_unk0x90;
	GameObject* m_textBoxManager;  // 0x94
	GameObject* m_npcTextManager;  // 0x98
	GameObject* m_bodypartManager; // 0x9c
	LegoU32 m_unk0xa0;
	LegoU32 m_unk0xa4;
	GameObject* m_secondaryFxManager; // 0xa8; deliberately not initialized here
	LegoU32 m_unk0xac;
	GameObject* m_menuManager; // 0xb0
	void* m_unk0xb4;
	GameObject* m_networkApp; // 0xb8
	GameObject* m_unk0xbc;
	GameObject* m_killSphereSupervisor; // 0xc0
	GameObject* m_reverbManager;        // 0xc4
	GameObject* m_unk0xc8;
	BitmapFontSet* m_fontSet;
	LegoU32 m_unk0xd0;
	void* m_unk0xd4;
	void* m_unk0xd8;
	LegoU32 m_unk0xdc;
	LegoS32 m_unk0xe0;
	LegoU32 m_unk0xe4;
	LegoU32 m_unk0xe8;
	LegoU32 m_unk0xec;
	InputController* m_controllers[8]; // 0xf0
	void* m_playerProfiles[8];         // 0x110
	undefined m_unk0x130[4];
	GameControllerName m_names[2]; // 0x134
	undefined m_unk0x15c[0x1e4 - 0x15c];
	GameControllerPairGroups m_pairGroups[2]; // 0x1e4
	LegoU32 m_graphicsQuality;                // 0x33c
	LegoU32 m_unk0x340;
	LegoU32 m_unk0x344;
	LegoU32 m_unk0x348;
	LegoU32 m_unk0x34c;
	GameControllerSelection m_selection; // 0x350
	LegoU32 m_unk0x370;
	GameControllerScore m_scores[3][25]; // 0x374
	undefined m_unk0xa7c[0xaa0 - 0xa7c];
	GameControllerItemBlock m_itemBlocks[39]; // 0xaa0
	GameControllerItem m_lastBlockItems[36];  // 0x7dcc
	LegoU32 m_blockState[2][4];               // 0x809c
	LegoU32 m_unk0x80bc[4];
	LegoU32 m_unk0x80cc;
	LegoU32 m_unk0x80d0[5];
	LegoU32 m_settingsCookie; // 0x80e4
	undefined m_unk0x80e8[0x817c - 0x80e8];
	GameControllerPlayerBlock m_playerBlocks[8]; // 0x817c
	undefined m_unk0x9b9c[0x9bdc - 0x9b9c];
	GameObject* m_screenPrinter; // 0x9bdc
};
DECOMP_STATIC_ASSERT(offsetof(GameController, m_names) == 0x134);
DECOMP_STATIC_ASSERT(offsetof(GameController, m_pairGroups) == 0x1e4);
DECOMP_STATIC_ASSERT(offsetof(GameController, m_scores) == 0x374);
DECOMP_STATIC_ASSERT(offsetof(GameController, m_lastBlockItems) == 0x7dcc);
DECOMP_STATIC_ASSERT(offsetof(GameController, m_blockState) == 0x809c);
DECOMP_STATIC_ASSERT(offsetof(GameController, m_playerBlocks) == 0x817c);
DECOMP_SIZE_ASSERT(GameController, 0x9be0);

struct GameControllerDispatch {
	GameObjectDispatchView m_base;
	LegoU32(__fastcall* m_size)(GameObject*, void*);
};
extern const GameControllerDispatch g_gameControllerDispatch;
extern GameController& g_gameController;
extern GameObjectTemplate g_gameControllerTemplate;
#endif
