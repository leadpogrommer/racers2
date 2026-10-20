#include "render/core/renderer.h"
#include "render/resources/rendererrecords.h"

namespace
{
struct RendererLoadableResource {
	struct Dispatch {
		void* m_unk0x00;
		LegoU32(__fastcall* m_load)(RendererLoadableResource*, void*);
	};
	const Dispatch* m_vtable;
	LegoU16 m_id;
	LegoU16 m_flags;
};
DECOMP_STATIC_ASSERT(offsetof(RendererLoadableResource, m_flags) == 6);
} // namespace

// STUB: LEGORACERS2 0x004c14f0
void DisplayRenderer::SetResourceLoadMode(LegoU32 p_mode)
{
	m_resourceLoadMode = p_mode;
	if (p_mode == 0) {
		for (LegoS32 index = 0; index < m_resourceReferenceCount; ++index) {
			RendererLoadableResource* resource = (RendererLoadableResource*) m_resourceReferences[index].m_resource;
			if ((resource->m_flags & 2) == 2) {
				resource->m_vtable->m_load(resource, NULL);
			}
		}
	}
}
