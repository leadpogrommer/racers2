#include "memory/memoryheap.h"
#include "render/core/renderer.h"

namespace
{
struct TextureLifetimeView {
	undefined m_unk0x00[0x0c];
	void(__fastcall* m_release)(RendererTextureRecord*, void*);
};

struct RendererShutdownView {
	RendererDispatchView::Policy m_initialize;
	void(__fastcall* m_shutdown)(DisplayRenderer*, void*);
};
} // namespace

// STUB: LEGORACERS2 0x004be330
void DisplayRenderer::ShutdownResources()
{
	LegoS32 index;
	for (index = 0; index < 800; index++) {
		RendererTextureRecord* texture = m_vtable->m_getTextureRecord(this, NULL, index);
		if ((texture->m_flags0x06 & 1) != 0) {
			if (FindTextureReferenceById(index, 1) == NULL) {
				texture = m_vtable->m_getTextureRecord(this, NULL, index);
				((const TextureLifetimeView*) texture->m_vtable)->m_release(texture, NULL);
				texture = m_vtable->m_getTextureRecord(this, NULL, index);
				texture->m_flags0x06 &= 0xfffe;
			}
			else {
				ReleaseTextureResourceReference(index, 1);
			}
		}
	}
	for (index = 0; index < 125; index++) {
		if (GetAuxiliaryResource(index)->m_inUse != 0) {
			ReleaseAuxiliaryResource(index);
		}
	}
	((const RendererShutdownView*) m_vtable)->m_shutdown(this, NULL);
	RendererResourceWorkList* workList = m_resourceWorkList;
	if (workList != NULL) {
		if (workList->m_elements != NULL) {
			GetMemoryHeap()->Free(workList->m_elements);
			workList->m_elements = NULL;
		}
		FreeHeapMemory(workList);
	}
	m_deferredDraws.ReleaseStorage();
	m_orderedDraws.ReleaseStorage();
	m_bucketedDraws.ReleaseStorage();
	GetMemoryHeap()->Free(m_resourceLookup);
	GetMemoryHeap()->Free(m_resources0xa0);
	GetMemoryHeap()->Free(m_resources0x98);
	FreeHeapMemory(m_resourceCache);
	FreeHeapMemory(m_resourceReferences);
	FreeHeapMemory(m_auxiliaryResources);
}

// STUB: LEGORACERS2 0x004c1050
void DisplayRenderer::ReleaseAuxiliaryResource(LegoS32 p_index)
{
	if (p_index == -1) {
		return;
	}
	RendererAuxiliaryResourceRecord* resource = &m_auxiliaryResources[p_index];
	if (resource->m_inUse == 0) {
		return;
	}
	resource->m_referenceCount--;
	if (resource->m_referenceCount != 0) {
		return;
	}
	if (resource->m_path[0] != '\0') {
		LegoS32 count = m_resourceCount0xa4;
		for (LegoS32 index = 0; index < count; index++) {
			if (m_resources0xa0[index] == resource) {
				m_resourceCount0xa4 = count - 1;
				m_resources0xa0[index] = m_resources0xa0[count - 1];
				break;
			}
		}
	}
	resource->m_inUse = 0;
}
