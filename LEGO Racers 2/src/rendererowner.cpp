#include "memoryheap.h"
#include "renderer.h"
#include "rendererbufferdispatch.h"

#include <new.h>
#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x0058b140
LegoU32 g_rendererStorage[sizeof(DisplayRenderer) / sizeof(LegoU32)];

DECOMP_STATIC_ASSERT(sizeof(g_rendererStorage) == sizeof(DisplayRenderer));

// GLOBAL: LEGORACERS2 0x00549dfc
DisplayRenderer* g_displayRenderer = (DisplayRenderer*) &g_rendererStorage;

DisplayRenderer* GetStaticDisplayRenderer()
{
	return (DisplayRenderer*) &g_rendererStorage;
}

static void ConstructStaticDisplayRenderer();
static void RegisterStaticDisplayRendererExit();
static void DestroyStaticDisplayRenderer();

// STUB: LEGORACERS2 0x004ab440
static void InitializeStaticDisplayRenderer()
{
	ConstructStaticDisplayRenderer();
	RegisterStaticDisplayRendererExit();
}

// STUB: LEGORACERS2 0x004ab450
static void ConstructStaticDisplayRenderer()
{
	new (g_rendererStorage) DisplayRenderer;
}

// STUB: LEGORACERS2 0x004ab460
static void RegisterStaticDisplayRendererExit()
{
	atexit(DestroyStaticDisplayRenderer);
}

// STUB: LEGORACERS2 0x004ab470
static void DestroyStaticDisplayRenderer()
{
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	LegoS32 index;
	for (index = 4999; index >= 0; --index) {
		RendererIndexBuffer* buffer = &renderer->m_indexRecords[index];
		buffer->m_vtable = &g_rendererIndexBufferDispatch;
		if (buffer->m_storage != NULL) {
			buffer->m_storage->ReleaseReference();
			buffer->m_storage = NULL;
		}
		buffer->ReleaseDeviceBuffer();
		buffer->m_flags &= ~1;
		// Retail's seven-slot base index table at 53551c is the tail of
		// the complete vertex table, beginning at its conditional-free slot.
		buffer->m_vtable = &g_rendererVertexBufferDispatch.m_legacyConditionalFree;
		if (buffer->m_unk0x08 == 1 && buffer->m_data != NULL) {
			GetMemoryHeap()->Free(buffer->m_data);
			buffer->m_data = NULL;
		}
	}
	for (index = 127; index >= 0; --index) {
		renderer->m_vertexStorage[index].ReleaseReference();
	}
	for (index = 4999; index >= 0; --index) {
		RendererVertexBuffer* buffer = &renderer->m_vertexRecords[index];
		buffer->m_vtable = &g_rendererVertexBufferDispatch;
		buffer->Release();
		if (buffer->m_unk0x24 != 0 && buffer->m_data != NULL) {
			GetMemoryHeap()->Free(buffer->m_data);
		}
	}
	// The final embedded-member destructor at retail 479030 is a bare RET.
}

struct StaticDisplayRendererInitialization {
	StaticDisplayRendererInitialization() { InitializeStaticDisplayRenderer(); }
};
static StaticDisplayRendererInitialization g_staticDisplayRendererInitialization;
