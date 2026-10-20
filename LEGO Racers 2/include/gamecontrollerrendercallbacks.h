#ifndef LR2_GAMECONTROLLERRENDERCALLBACKS_H
#define LR2_GAMECONTROLLERRENDERCALLBACKS_H

#include "types.h"

class GameController;
struct GameObjectModelBinding;
struct PhysicalLifecycleState;
struct RendererAuxiliaryResourceRecord;
struct RendererResourceCacheEntry;

LegoU32 __cdecl CrystalMaterialResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
LegoU32 __cdecl CrystalVariantMaterialResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
LegoU32 __cdecl SlowCrystalMaterialResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
LegoU32 __cdecl LightShaftMaterialResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
LegoU32 __cdecl TrackTextureResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
LegoU32 __cdecl PortalMaterialResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
void __fastcall RegisterTrackTextureAnimation(
	GameController*,
	void*,
	GameObjectModelBinding*,
	PhysicalLifecycleState*,
	LegoS32
);
void __fastcall UnregisterTrackTextureAnimation(GameController*, void*, GameObjectModelBinding*);
void __fastcall SelectResourceCacheAnimationFraction(RendererResourceCacheEntry*, void*, LegoU32);

#endif
