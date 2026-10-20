#include "game/character/bodypartmanager.h"

#include <stdio.h>
#include <string.h>

struct BodypartLookupTextureEntry {
	CharBuffer128 m_path;
	LegoS32 m_textureIndex;
};

struct BodypartLookupHeadEntry {
	CharBuffer128 m_path;
	LegoU32 m_flags;
	ModelResource* m_model;
};

DECOMP_SIZE_ASSERT(BodypartLookupTextureEntry, 0x84);
DECOMP_SIZE_ASSERT(BodypartLookupHeadEntry, 0x88);

// STUB: LEGORACERS2 0x0044daa0
void __cdecl StripBodypartPathAndExtension(LegoChar* p_destination, const LegoChar* p_path)
{
	const LegoChar* base = strrchr(p_path, '\\');
	LegoChar* extension;

	if (base != NULL) {
		p_path = base + 1;
	}
	strcpy(p_destination, p_path);
	extension = strrchr(p_destination, '.');
	if (extension != NULL) {
		*extension = '\0';
	}
}

// STUB: LEGORACERS2 0x0044db00
LegoS32 BodypartManager::FindHeadModel(const LegoChar* p_name, LegoU32* p_species)
{
	LegoChar wanted[64];
	LegoChar found[64];
	LegoS32 i;
	sprintf(wanted, "%s head", p_name);
	for (i = 0; i < m_headModelCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupHeadEntry*) m_headModels)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			*p_species = 0;
			return i;
		}
	}
	for (i = 0; i < m_alienHeadModelCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupHeadEntry*) m_alienHeadModels)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			*p_species = 1;
			return i;
		}
	}
	for (i = 0; i < m_ramaHeadModelCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupHeadEntry*) m_ramaHeadModels)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			*p_species = 2;
			return i;
		}
	}
	return -1;
}

// FUNCTION: LEGORACERS2 0x0044dcf0
LegoS32 BodypartManager::FindTorsoTexture(const LegoChar* p_name, LegoU32* p_species)
{
	LegoChar wanted[64];
	LegoChar found[64];
	LegoS32 i;
	sprintf(wanted, "%s torso", p_name);
	for (i = 0; i < m_torsoTextureCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupTextureEntry*) m_torsoTextures)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			*p_species = 0;
			return i;
		}
	}
	for (i = 0; i < m_alienTorsoTextureCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupTextureEntry*) m_alienTorsoTextures)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			*p_species = 1;
			return i;
		}
	}
	for (i = 0; i < m_ramaTorsoTextureCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupTextureEntry*) m_ramaTorsoTextures)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			*p_species = 2;
			return i;
		}
	}
	return -1;
}

// FUNCTION: LEGORACERS2 0x0044dec0
LegoS32 BodypartManager::FindLegTexture(const LegoChar* p_name)
{
	LegoChar wanted[64];
	LegoChar found[64];
	LegoS32 i;
	sprintf(wanted, "%s legs", p_name);
	for (i = 0; i < m_legTextureCount; ++i) {
		StripBodypartPathAndExtension(found, ((BodypartLookupTextureEntry*) m_legTextures)[i].m_path.m_data);
		if (strcmp(found, wanted) == 0) {
			return i;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x0044df70
void __fastcall FindNamedCharacterBodyparts(BodypartManager* p_manager, void*, const LegoChar* p_name, LegoU32* p_parts)
{
	LegoU32 headSpecies;
	LegoU32 torsoSpecies;
	p_parts[0] = p_manager->FindHeadModel(p_name, &headSpecies);
	p_parts[2] = p_manager->FindTorsoTexture(p_name, &torsoSpecies);
	if (torsoSpecies == 0) {
		p_parts[3] = p_manager->FindLegTexture(p_name);
		p_parts[1] = p_parts[0];
	}
	else {
		p_parts[1] = torsoSpecies == 1 ? -1 : -2;
	}
}
