#include "memoryheap.h"
#include "modelresource.h"
#include "modelresourcecache.h"
#include "modelresourcechunks.h"
#include "modelshadow.h"
#include "modelskin.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"

// STUB: LEGORACERS2 0x004a7a70
LegoBool32 ModelResourceCache::Release(ModelResource* p_model)
{
	LegoU32 index;
	for (index = 0; index < m_count; ++index) {
		if (m_entries[index].m_model == p_model) {
			break;
		}
	}
	if (m_entries[index].m_references > 1) {
		--m_entries[index].m_references;
		return FALSE;
	}
	--m_count;
	for (; index < m_count; ++index) {
		m_entries[index] = m_entries[index + 1];
	}
	m_entries[index].m_model = NULL;
	m_entries[index].m_references = 0;
	if (p_model != NULL) {
		p_model->~ModelResource();
		FreeHeapMemory(p_model);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004a7ba0
ModelResource::~ModelResource()
{
	m_resources.ReleaseRendererReferences();
	if (m_collisionData != NULL) {
		ReleaseCollisionResource(&m_collisionData);
		m_collisionData = NULL;
	}
	// Retail calls 4a9630(0, 0, 1); its zero first argument takes this release path.
	ReleaseModelShadowData(this, NULL);
	if (m_skin != NULL) {
		m_skin->ReleaseStorage();
		FreeHeapMemory(m_skin);
		m_skin = NULL;
	}
	if (m_groups != NULL) {
		for (LegoS32 index = m_groupCount - 1; index >= 0; --index) {
			m_groups[index].ReleaseStorage();
		}
		FreeHeapMemory(m_groups);
	}
	((const RendererMainDispatch*) g_displayRenderer->m_vtable)->m_diagnostic((const LegoChar*) this);
	m_resources.FreeStorage();
	if (m_bounds != NULL) {
		FreeHeapMemory(m_bounds);
	}
}
