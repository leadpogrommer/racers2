#include "render/models/modelresourcecache.h"

#include "core/application/application.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x00579168
ModelResourceCache g_modelResourceCache;

// STUB: LEGORACERS2 0x004a7900
LegoBool32 ModelResourceCacheEntry::MatchesPath(const LegoChar* p_path) const
{
	if (GetApplication()->IsDevelopmentMode() == FALSE) {
		return _stricmp(p_path, m_path) == 0;
	}
	LegoU32 length = strlen(p_path);
	LegoU32 storedLength = strlen(m_path);
	if (_strnicmp(p_path, m_path, length) != 0) {
		return FALSE;
	}
	if (length == storedLength) {
		return TRUE;
	}
	return m_path[storedLength - 1] == '~' && length == storedLength - 1;
}

// FUNCTION: LEGORACERS2 0x004a79a0
void ModelResourceCacheEntry::Assign(ModelResource* p_model, const LegoChar* p_path)
{
	m_model = p_model;
	strcpy(m_path, p_path);
	m_references = 1;
}

// STUB: LEGORACERS2 0x004a79e0
ModelResource* ModelResourceCache::Find(const LegoChar* p_path)
{
	for (LegoU32 index = 0; index < m_count; ++index) {
		ModelResourceCacheEntry* entry = &m_entries[index];
		if (entry->m_model == NULL) {
			return NULL;
		}
		if (entry->MatchesPath(p_path) != FALSE) {
			return entry->m_model;
		}
	}
	return NULL;
}

// FUNCTION: LEGORACERS2 0x004a7a30
void ModelResourceCache::Insert(ModelResource* p_model, const LegoChar* p_path)
{
	LegoU32 index;
	for (index = 0; index < 512; ++index) {
		if (m_entries[index].m_model == NULL) {
			break;
		}
	}
	// Retail does not guard a full cache before assigning the selected entry.
	m_entries[index].Assign(p_model, p_path);
	++m_count;
}

// FUNCTION: LEGORACERS2 0x004a7b20
void ModelResourceCache::AddReference(ModelResource* p_model)
{
	LegoU32 index;
	for (index = 0; index < m_count; ++index) {
		if (m_entries[index].m_model == p_model) {
			break;
		}
	}
	// The caller must provide a model already present in this cache.
	++m_entries[index].m_references;
}

// STUB: LEGORACERS2 0x004a7b70
void InitializeModelResourceCache()
{
	g_modelResourceCache.m_count = 0;
	for (LegoU32 index = 0; index < 512; ++index) {
		g_modelResourceCache.m_entries[index].m_model = NULL;
		g_modelResourceCache.m_entries[index].m_references = 0;
	}
}

struct InitializeModelResourceCacheAtStartup {
	InitializeModelResourceCacheAtStartup() { InitializeModelResourceCache(); }
};
static InitializeModelResourceCacheAtStartup g_initializeModelResourceCacheAtStartup;
