#include "memory/memoryheap.h"
#include "platform/system/platformfpu.h"
#include "render/core/renderer.h"
#include "render/device/displayconfig.h"

namespace
{
struct RendererBufferReleaseDispatch {
	void(__fastcall* m_release)(void*, void*);
};
} // namespace

// STUB: LEGORACERS2 0x004ad060
void DisplayRenderer::Shutdown()
{
	ConfigureStreamResources(0);
	for (LegoS32 i = 0; i < 5000; i++) {
		RendererVertexBuffer* buffer = &m_vertexRecords[i];
		if (buffer->m_storage != NULL) {
			const RendererBufferReleaseDispatch* dispatch = (const RendererBufferReleaseDispatch*) buffer->m_vtable;
			dispatch->m_release(buffer, NULL);
		}
	}
	for (LegoS32 index = 0; index < 5000; index++) {
		RendererIndexBuffer* buffer = &m_indexRecords[index];
		if ((buffer->m_flags & 1) != 0) {
			const RendererBufferReleaseDispatch* dispatch = (const RendererBufferReleaseDispatch*) buffer->m_vtable;
			dispatch->m_release(buffer, NULL);
		}
	}
	for (LegoS32 viewport = 0; viewport < 16; viewport++) {
		m_vtable->m_releaseViewport(this, NULL, viewport);
	}
	if (m_device != NULL) {
		m_device->Release();
		m_device = NULL;
	}
	if (m_direct3D != NULL) {
		m_direct3D->Release();
		m_direct3D = NULL;
	}
	RestoreFloatingPointControl();
	if (m_restoreCursorOnShutdown == 1) {
		ShowCursor(TRUE);
	}
	if (g_displayConfiguration != NULL) {
		FreeHeapMemory(g_displayConfiguration);
		g_displayConfiguration = NULL;
	}
}

// FUNCTION: LEGORACERS2 0x004ae5a0
void DisplayRenderer::ReleaseViewport(LegoU32 p_index)
{
	m_viewports[p_index].m_inUse = 0;
}
