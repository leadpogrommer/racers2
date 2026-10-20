#include "render/resources/rendererresourcecallbacks.h"

#include "render/core/renderer.h"

#include <string.h>

RendererAuxiliaryResourceRecord* __fastcall FindAuxiliaryResourceByPath(
	DisplayRenderer* p_renderer,
	void*,
	const LegoChar* p_path
)
{
	for (LegoS32 index = 0; index < 125; ++index) {
		RendererAuxiliaryResourceRecord* record = &p_renderer->m_auxiliaryResources[index];
		if (record->m_inUse != 0 && strcmp(record->m_path, p_path) == 0) {
			return record;
		}
	}
	return NULL;
}

LegoBool32 __fastcall ClearAuxiliaryResourceCallback(
	RendererAuxiliaryResourceRecord* p_record,
	void*,
	RendererAuxiliaryResourceCallback p_callback
)
{
	if (p_record->m_path[0] == '\0' || p_record->m_callback != p_callback) {
		return FALSE;
	}
	p_record->m_callback = NULL;
	return TRUE;
}

// STUB: LEGORACERS2 0x004c10d0
void __fastcall RegisterResourceCallback(
	DisplayRenderer* p_renderer,
	void*,
	const LegoChar* p_path,
	RendererAuxiliaryResourceCallback p_callback
)
{
	RendererResourceLookupEntry* entry = &p_renderer->m_resourceLookup[p_renderer->m_resourceLookupCount];
	entry->m_callback = p_callback;
	memcpy(entry->m_path, p_path, strlen(p_path) + 1);
	p_renderer->m_resourceLookupCount++;

	RendererAuxiliaryResourceRecord* record = FindAuxiliaryResourceByPath(p_renderer, NULL, p_path);
	if (record != NULL) {
		record->SetCallback(p_callback);
	}
}

// STUB: LEGORACERS2 0x004c1150
void __fastcall UnregisterResourceCallback(
	DisplayRenderer* p_renderer,
	void*,
	RendererAuxiliaryResourceCallback p_callback
)
{
	LegoS32 index = 0;
	while (index < p_renderer->m_resourceLookupCount && p_renderer->m_resourceLookup[index].m_callback != p_callback) {
		index++;
	}
	if (index >= p_renderer->m_resourceLookupCount) {
		return;
	}

	RendererResourceLookupEntry* entry = &p_renderer->m_resourceLookup[index];
	RendererAuxiliaryResourceRecord* record = FindAuxiliaryResourceByPath(p_renderer, NULL, entry->m_path);
	if (record != NULL) {
		ClearAuxiliaryResourceCallback(record, NULL, p_callback);
	}

	LegoS32 newCount = --p_renderer->m_resourceLookupCount;
	if (index < newCount) {
		p_renderer->m_resourceLookup[index] = p_renderer->m_resourceLookup[newCount];
	}
}
