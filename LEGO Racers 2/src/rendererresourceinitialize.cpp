#include "memoryheap.h"
#include "renderer.h"

#include <string.h>

// STUB: LEGORACERS2 0x004be0a0
LegoBool32 DisplayRenderer::InitializeResources()
{
	m_unk0x4d0 = 8;
	m_unk0x4d4 = 8;
	m_unk0x4d8 = 3;
	m_auxiliaryResources =
		(RendererAuxiliaryResourceRecord*) AllocateHeapMemory(125 * sizeof(RendererAuxiliaryResourceRecord));
	if (m_auxiliaryResources != NULL) {
		for (LegoS32 i = 0; i < 125; i++) {
			m_auxiliaryResources[i].InitializeHeader();
			m_auxiliaryResources[i].m_inUse = 0;
			m_auxiliaryResources[i].m_referenceCount = 0;
		}
	}
	m_resourceReferences = (RendererResourceRef*) AllocateHeapMemory(832 * sizeof(RendererResourceRef));
	if (m_resourceReferences != NULL) {
		for (LegoS32 reference = 0; reference < 832; reference++) {
			m_resourceReferences[reference].m_resource = NULL;
			m_resourceReferences[reference].m_referenceCount = 0;
		}
	}
	m_resourceCache = (RendererResourceCacheEntry*) AllocateHeapMemory(32 * sizeof(RendererResourceCacheEntry));
	if (m_resourceCache != NULL) {
		for (LegoS32 cached = 0; cached < 32; cached++) {
			m_resourceCache[cached].InitializeRecord();
		}
	}
	m_resources0x98 = (RendererResourceRef**) GetMemoryHeap()->Allocate(0x80, "Animated textures");
	m_resources0xa0 = (void**) GetMemoryHeap()->Allocate(0x80, "Animated materials");
	m_resourceLookup = (RendererResourceLookupEntry*) GetMemoryHeap()->Allocate(0x180, "Material animators");
	m_resourceLookupCount = 0;
	m_resourceCount0xa4 = 0;
	m_resourceCount0x9c = 0;
	m_resourceReferenceCount = 0;
	m_bucketedDraws.Initialize(0x800, 3000, 2, 0);
	m_orderedDraws.Initialize(0x321, 0x200, 0, 1);
	m_deferredDraws.Initialize(2, 0x100, 0, 0);
	m_drawLookupTables[0] = &m_bucketedDraws;
	m_drawLookupTables[1] = &m_orderedDraws;
	m_drawLookupTables[2] = &m_deferredDraws;
	m_resourceLoadMode = 0;
	m_unk0x4dc = 0;
	memset(m_textureHash, 0, sizeof(m_textureHash));
	m_resourceWorkList = (RendererResourceWorkList*) AllocateHeapMemory(sizeof(RendererResourceWorkList));
	if (m_resourceWorkList != NULL) {
		m_resourceWorkList->m_elements = NULL;
		m_resourceWorkList->m_stride = 4;
		m_resourceWorkList->m_count = 0;
		m_resourceWorkList->m_capacity = 0;
	}
	m_resourceWorkList->m_capacity = 16;
	m_resourceWorkList->m_elements = GetMemoryHeap()->Allocate(
		m_resourceWorkList->m_stride * m_resourceWorkList->m_capacity,
		"Elements in a cSimpleList"
	);
	m_unk0x4c0 = 0;
	m_unk0x4b4 = 0;
	return m_vtable->m_initialize(this, NULL);
}

// FUNCTION: LEGORACERS2 0x004be320
RendererAuxiliaryResourceRecord* RendererAuxiliaryResourceRecord::InitializeHeader()
{
	m_mode = 0;
	m_callback = NULL;
	m_path[0] = '\0';
	return this;
}
