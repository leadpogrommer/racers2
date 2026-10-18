#include "decomp.h"
#include "gameobjectrenderprep.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"

#include <stddef.h>

struct GameObjectViewportView {
	undefined m_prefix[0x1b0];
	LegoS32 m_left;
	LegoS32 m_top;
	LegoS32 m_width;
	LegoS32 m_height;
	undefined m_unk0x1c0[4];
	LegoU32 m_viewportIndex;
};
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewportView, m_left) == 0x1b0);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewportView, m_width) == 0x1b8);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewportView, m_viewportIndex) == 0x1c4);

// STUB: LEGORACERS2 0x004ba810
void __fastcall SetGameObjectViewport(
	void* p_object,
	void*,
	LegoS32 p_left,
	LegoS32 p_top,
	LegoS32 p_width,
	LegoS32 p_height
)
{
	GameObjectViewportView* view = (GameObjectViewportView*) p_object;
	DisplayRenderer* renderer = g_displayRenderer;
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) renderer->m_vtable;
	dispatch->m_updateViewport(renderer, NULL, view->m_viewportIndex, p_left, p_top, p_width, p_height);
	view->m_left = p_left;
	view->m_top = p_top;
	view->m_width = p_width;
	view->m_height = p_height;
}
