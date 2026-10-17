#include "rendererrecords.h"

#include "renderer.h"

#include <string.h>

namespace
{
typedef LegoBool32(__fastcall* SetCacheEntryPath)(RendererResourceCacheEntry*, void*, const LegoChar*);
typedef void(__fastcall* LoadCacheEntry)(RendererResourceCacheEntry*, void*);

struct RendererResourceCacheEntryDispatch {
	SetCacheEntryPath m_setPath;
	LoadCacheEntry m_load;
};

typedef LegoBool32(__fastcall* IsTextureResourceAvailable)(void*, void*);
typedef LegoBool32(__fastcall* LoadTextureResource)(void*, void*);

struct RendererTextureRecordDispatch {
	undefined m_unk0x00[0x04];
	LoadTextureResource m_load;
	undefined m_unk0x08[0x10 - 0x08];
	IsTextureResourceAvailable m_isAvailable;
};

typedef RendererTextureRecord*(__fastcall* GetTextureRecord)(DisplayRenderer*, void*, LegoU32);

struct DisplayRendererTextureLookupDispatch {
	undefined m_unk0x00[0x114];
	GetTextureRecord m_getTextureRecord;
};
} // namespace

// STUB: LEGORACERS2 0x00479490
RendererDefaultRecord::RendererDefaultRecord()
{
	m_unk0x00 = 0;
	m_unk0x04 = 0;
	m_unk0x06 = 0;
	m_unk0x07 = 1;
	for (LegoU32 i = 0; i < 4; i++) {
		m_slots[i].m_value = 0xffffffff;
		m_slots[i].m_mask = 0xffff;
		m_slots[i].m_kind = 0x0f;
		m_slots[i].m_flags = 0x03;
	}
}

// FUNCTION: LEGORACERS2 0x004abe80
LegoU32 RendererTextureRecord::GetFlag0x90()
{
	return (m_flags0x90 >> 1) & 1;
}

// FUNCTION: LEGORACERS2 0x004abea0
RendererArrayMarker::RendererArrayMarker()
{
	m_value = 7;
}

// STUB: LEGORACERS2 0x004bfb50
RendererTextureRecord* DisplayRenderer::FindOrCreateTextureRecord(
	const LegoChar* p_path,
	LegoU32 p_kind,
	LegoS32 p_index,
	LegoU32 p_resourceMode,
	LegoU32 p_auxiliaryMode,
	LegoBool32 p_load
)
{
	LegoU32 pathHash = GetTexturePathHash(p_path);
	if (*p_path == '\0') {
		p_resourceMode = 0;
	}
	else if (p_resourceMode == 1) {
		RendererTextureRecord* record = m_textureHash[pathHash];
		if (record != NULL && record->MatchesResource(p_path, pathHash, (LegoS32) p_kind)) {
			return record;
		}

		DisplayRendererTextureLookupDispatch* dispatch = (DisplayRendererTextureLookupDispatch*) m_vtable;
		for (LegoS32 i = 0; i < 800; i++) {
			record = dispatch->m_getTextureRecord(this, NULL, (LegoU32) i);
			if (record->MatchesResource(p_path, pathHash, (LegoS32) p_kind)) {
				return record;
			}
		}
	}

	DisplayRendererTextureLookupDispatch* dispatch = (DisplayRendererTextureLookupDispatch*) m_vtable;
	for (LegoS32 i = 0; i < 800; i++) {
		RendererTextureRecord* record = dispatch->m_getTextureRecord(this, NULL, (LegoU32) i);
		if ((record->m_flags0x06 & 1) == 0) {
			record->m_flags0x06 |= 3;
			record->SetResourcePath(p_path, (LegoU16) p_kind, (LegoU8) p_resourceMode);
			record->m_resourceId = (LegoS16) i;

			if (p_load == FALSE || ((RendererTextureRecordDispatch*) record->m_vtable)->m_load(record, NULL) != FALSE) {
				m_textureHash[pathHash] = record;
				return record;
			}
			record->m_flags0x06 &= 0xfffe;
			break;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004bfce0
void RendererTextureRecord::SetResourcePath(const LegoChar* p_path, LegoU16 p_type, LegoU8 p_enabled)
{
	m_flags0x90 = (m_flags0x90 & 0xfffe) | (p_enabled & 1);
	strcpy(m_resourcePath, p_path);
	m_flags0x90 &= 0xfffd;
	m_resourceType = p_type;
	m_resourceIndex = 0;
	m_flags0x06 = (LegoU16) ((GetTexturePathHash(m_resourcePath) << 6) | (m_flags0x06 & 0xc03f));
}

// FUNCTION: LEGORACERS2 0x004bffb0
LegoU32 GetTexturePathHash(const LegoChar* p_path)
{
	LegoU32 hash = 0;
	while (*p_path != '\0') {
		hash += (LegoU32) (LegoS8) *p_path;
		p_path++;
	}
	return hash & 0xff;
}

// FUNCTION: LEGORACERS2 0x004bffd0
LegoBool32 RendererTextureRecord::MatchesResource(
	const LegoChar* p_path,
	LegoU32 p_pathHash,
	LegoS32 p_resourceType
) const
{
	if ((m_flags0x06 & 1) == 0 || (m_flags0x90 & 1) == 0 || (LegoU32) ((m_flags0x06 >> 6) & 0xff) != p_pathHash) {
		return FALSE;
	}
	return strcmp(m_resourcePath, p_path) == 0 && (LegoS16) m_resourceType == p_resourceType;
}

// STUB: LEGORACERS2 0x004c09a0
RendererResourceCacheEntry* DisplayRenderer::FindOrAllocateTextureResource(const LegoChar* p_path, LegoBool32 p_loadNow)
{
	LegoS32 i;
	for (i = 0; i < 32; i++) {
		RendererResourceCacheEntry* entry = &m_resourceCache[i];
		if ((entry->m_flags0x06 & 1) != 0 && strcmp(entry->m_path, p_path) == 0) {
			return entry;
		}
	}

	for (i = 0; i < 32; i++) {
		RendererResourceCacheEntry* entry = &m_resourceCache[i];
		if ((entry->m_flags0x06 & 1) == 0) {
			entry->m_flags0x06 |= 3;
			RendererResourceCacheEntryDispatch* dispatch = (RendererResourceCacheEntryDispatch*) entry->m_vtable;
			dispatch->m_setPath(entry, NULL, p_path);
			if (p_loadNow != FALSE) {
				dispatch->m_load(entry, NULL);
			}
			return entry;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004c0a60
LegoS32 DisplayRenderer::AddTextureReference(void* p_resource, LegoU16 p_resourceType)
{
	if (p_resource == NULL) {
		return -1;
	}

	RendererResourceRef* reference = FindTextureReference(p_resource);
	if (reference == NULL) {
		reference = &m_resourceReferences[m_resourceReferenceCount++];
		reference->m_resource = p_resource;
		reference->m_referenceCount = 0;
		reference->m_resourceType = p_resourceType;
		RendererTextureRecordDispatch* dispatch = (RendererTextureRecordDispatch*) *(void**) p_resource;
		if (dispatch->m_isAvailable(p_resource, NULL) != FALSE) {
			m_resources0x98[m_resourceCount0x9c++] = reference;
		}
	}

	reference->m_referenceCount++;
	return (LegoS32) * (LegoS16*) ((LegoU8*) p_resource + 4);
}

// STUB: LEGORACERS2 0x004c0b30
RendererResourceRef* DisplayRenderer::FindTextureReference(void* p_resource)
{
	for (LegoS32 i = 0; i < m_resourceReferenceCount; i++) {
		RendererResourceRef* reference = &m_resourceReferences[i];
		if (reference->m_resource == p_resource && reference->m_referenceCount != 0) {
			return reference;
		}
	}
	return NULL;
}
