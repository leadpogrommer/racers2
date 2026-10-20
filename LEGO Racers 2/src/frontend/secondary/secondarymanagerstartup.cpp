#include "audio/service/audioservice.h"
#include "core/application/commandline.h"
#include "frontend/overlay/globaloverlayglyph.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/objects/gameobjectrenderprep.h"
#include "platform/window/gamewindow.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/fonts/bitmapfontset.h"

#include <math.h>

void __fastcall BeginSecondaryManagerFallback(SecondaryManager* p_manager, void*);
void FUN_0042a860();

void __fastcall BeginSecondaryManagerFallback(SecondaryManager* p_manager, void*);
void FUN_0042a860();

// STUB: LEGORACERS2 0x004279b0
void SecondaryManager::BeginStartup()
{
	if (g_gameController.m_unk0x68 == 0) {
		if (PrepareSplash(0) == FALSE) {
			m_state = 1;
			BeginSecondaryManagerFallback(this, NULL);
			return;
		}
		g_gameController.PlayStartupMovies();
	}

	m_state = 0;
	if (m_requestedState == -1) {
		m_requestedState = 0;
	}
}

// STUB: LEGORACERS2 0x0042a6f0
void __fastcall BeginSecondaryManagerFallback(SecondaryManager* p_manager, void*)
{
	DisplayRenderer* renderer = g_displayRenderer;
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) renderer->m_vtable;
	dispatch->m_setDeviceResourceMode(renderer, NULL, 0);
	g_gameController.m_fontSet->Initialize(2);
	((GlobalOverlayGlyphReplacementManagerView*) g_gameController.m_unk0xb4)->LoadIconTextures();
	UpdateGameObjectRenderGeometry(p_manager->m_unk0x110, NULL);
	if (dispatch->m_setDeviceResourceMode(renderer, NULL, 2) == FALSE) {
		FUN_0042a860();
	}
}

// STUB: LEGORACERS2 0x0042a740
void SecondaryManager::RestoreStartupRendering()
{
	*(LegoU32*) ((LegoU8*) m_unk0x110 + 0x1c0) = 0;
	((GlobalOverlayGlyphReplacementManagerView*) g_gameController.m_unk0xb4)->ReleaseIconTextures();
	g_gameController.m_fontSet->Release(2);
}

// STUB: LEGORACERS2 0x0042a860
void FUN_0042a860()
{
	g_deferredWindowDestruction = 1;
}
