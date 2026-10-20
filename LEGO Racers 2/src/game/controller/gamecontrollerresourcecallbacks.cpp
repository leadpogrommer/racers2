#include "game/character/charactervisual.h"
#include "game/controller/gamecontrollerrendercallbacks.h"
#include "render/core/renderer.h"
#include "render/resources/rendererresourcecallbacks.h"

// STUB: LEGORACERS2 0x0045f610
void RegisterGameControllerCallbacks()
{
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"CRYSTL",
		(RendererAuxiliaryResourceCallback) &CrystalMaterialResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"CRYSTL1",
		(RendererAuxiliaryResourceCallback) &CrystalVariantMaterialResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"CRYSTL2",
		(RendererAuxiliaryResourceCallback) &SlowCrystalMaterialResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"SHIELD",
		(RendererAuxiliaryResourceCallback) &CharacterShieldResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"MSHIELD",
		(RendererAuxiliaryResourceCallback) &CharacterSheenResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"SHAFTS",
		(RendererAuxiliaryResourceCallback) &LightShaftMaterialResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"TRACKS",
		(RendererAuxiliaryResourceCallback) &TrackTextureResourceCallback
	);
	RegisterResourceCallback(
		g_displayRenderer,
		NULL,
		"PORTAL",
		(RendererAuxiliaryResourceCallback) &PortalMaterialResourceCallback
	);
}

// STUB: LEGORACERS2 0x0045f6c0
void UnregisterGameControllerCallbacks()
{
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &PortalMaterialResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &TrackTextureResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &LightShaftMaterialResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &CharacterSheenResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &CharacterShieldResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &SlowCrystalMaterialResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &CrystalVariantMaterialResourceCallback
	);
	UnregisterResourceCallback(
		g_displayRenderer,
		NULL,
		(RendererAuxiliaryResourceCallback) &CrystalMaterialResourceCallback
	);
}
