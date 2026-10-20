#include "render/core/renderer.h"

// Offset view of the index-record table, used only for its real callbacks.
struct StreamingIndexDispatch {
	typedef void(__fastcall* Release)(RendererIndexBuffer*, void*);
	typedef void(__fastcall* Policy)(RendererIndexBuffer*, void*, LegoU32);
	Release m_release; // 0x00
	undefined m_unk0x04[0x10 - 0x04];
	Policy m_setPolicy; // 0x10
};
DECOMP_STATIC_ASSERT(offsetof(StreamingIndexDispatch, m_setPolicy) == 0x10);

// STUB: LEGORACERS2 0x004ec2c0
void DisplayRenderer::ConfigureStreamResources(LegoU32 p_enabled)
{
	if (p_enabled == 1) {
		RendererAuxiliaryResourceRecord resource;
		LegoU32 mode = m_vtable->m_usesHardwareVertexProcessing(this, NULL) == 1 ? 4 : 3;
		resource.InitializeMaterial(mode, 0, 0xffffffff, 0, 0.0f, 0.0f, 0.0f);
		m_streamResourceId = AcquireAuxiliaryResource(&resource, 0);
		m_vertexBufferCapacity = 0x2420;
		m_streamingIndexBuffer = m_vtable->m_createIndexBuffer(this, NULL, 1, 0x4a60);
		const StreamingIndexDispatch* dispatch = (const StreamingIndexDispatch*) m_streamingIndexBuffer->m_vtable;
		dispatch->m_setPolicy(m_streamingIndexBuffer, NULL, 1);
		return;
	}
	const StreamingIndexDispatch* dispatch = (const StreamingIndexDispatch*) m_streamingIndexBuffer->m_vtable;
	dispatch->m_setPolicy(m_streamingIndexBuffer, NULL, 0);
	// Reload after the policy callback as the original does.
	dispatch = (const StreamingIndexDispatch*) m_streamingIndexBuffer->m_vtable;
	dispatch->m_release(m_streamingIndexBuffer, NULL);
}
