#ifndef LR2_MODELRESOURCECACHE_H
#define LR2_MODELRESOURCECACHE_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct ModelResource;

struct ModelResourceCacheEntry {
	ModelResource* m_model;
	LegoChar m_path[128];
	LegoU32 m_references;

	LegoBool32 MatchesPath(const LegoChar* p_path) const;
	void Assign(ModelResource* p_model, const LegoChar* p_path);
};
DECOMP_SIZE_ASSERT(ModelResourceCacheEntry, 0x88);
DECOMP_STATIC_ASSERT(offsetof(ModelResourceCacheEntry, m_path) == 4);
DECOMP_STATIC_ASSERT(offsetof(ModelResourceCacheEntry, m_references) == 0x84);

struct ModelResourceCache {
	LegoU32 m_count;
	ModelResourceCacheEntry m_entries[512];

	ModelResource* Find(const LegoChar* p_path);
	void Insert(ModelResource* p_model, const LegoChar* p_path);
	void AddReference(ModelResource* p_model);
};
DECOMP_SIZE_ASSERT(ModelResourceCache, 0x11004);
DECOMP_STATIC_ASSERT(offsetof(ModelResourceCache, m_entries) == 4);

extern ModelResourceCache g_modelResourceCache;
void InitializeModelResourceCache();

#endif
