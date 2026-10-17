#include "renderer.h"

// Reserve the actual renderer allocation without invoking incomplete default
// construction of its records/dispatch. This is its eventual placement-new
// destination; it is not a second copy of renderer fields. The current entry
// point never calls renderer methods. Connect construction before startup.
// GLOBAL: LEGORACERS2 0x0058b140
LegoU32 g_rendererStorage[sizeof(DisplayRenderer) / sizeof(LegoU32)];

DECOMP_STATIC_ASSERT(sizeof(g_rendererStorage) == sizeof(DisplayRenderer));

// GLOBAL: LEGORACERS2 0x00549dfc
DisplayRenderer* g_displayRenderer = (DisplayRenderer*) &g_rendererStorage;

DisplayRenderer* GetStaticDisplayRenderer()
{
	return (DisplayRenderer*) &g_rendererStorage;
}
