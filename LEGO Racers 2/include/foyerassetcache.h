#ifndef LR2_FOYERASSETCACHE_H
#define LR2_FOYERASSETCACHE_H

#include "gamemanager.h"

struct FoyerAssetCache {
	GameObject* m_sparky;
	LegoS32 m_sparkyReferences;
	GameObject* m_defaultSize;
	LegoS32 m_defaultSizeReferences;
	GameObject* m_rigel;
	LegoS32 m_rigelReferences;
	GameObject* m_rocketRacer;
	LegoS32 m_rocketRacerReferences;
	GameObject* m_player;
	LegoS32 m_playerReferences;
	GameObject* m_intro;
	LegoS32 m_introReferences;
};

DECOMP_SIZE_ASSERT(FoyerAssetCache, 0x30);

FoyerAssetCache* __cdecl GetFoyerAssetCache();
void* __stdcall AcquireFoyerAsset(LegoU32 p_type);
void __fastcall ReleaseFoyerAsset(FoyerAssetCache*, void*, LegoU32 p_type);

#endif // LR2_FOYERASSETCACHE_H
