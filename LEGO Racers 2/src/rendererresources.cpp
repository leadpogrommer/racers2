#include "renderer.h"
#include "rendererrecords.h"
#include "winfile.h"

#include <string.h>

extern "C" char* __cdecl _strupr(char* p_text);

namespace
{
typedef LegoU32(__fastcall* ResourceKindPolicy)(DisplayRenderer*, void*, LegoU32);
typedef void*(__fastcall* GetTextureRecord)(DisplayRenderer*, void*, LegoU32);
typedef void*(__fastcall* CreateExternalTexture)(DisplayRenderer*, void*, const LegoChar*, LegoU32, LegoU16);
typedef LegoBool32(__fastcall* HighResolutionPolicy)(DisplayRenderer*, void*);

struct RendererTextureFactoryDispatch {
	undefined m_unk0x00[0x6c];
	ResourceKindPolicy m_resourceKindPolicy;
	undefined m_unk0x70[0x114 - 0x70];
	GetTextureRecord m_getTextureRecord;
	CreateExternalTexture m_createExternalTexture;
	undefined m_unk0x11c[0x178 - 0x11c];
	HighResolutionPolicy m_highResolutionPolicy;
};
} // namespace

// STUB: LEGORACERS2 0x004c0510
void RendererAuxiliaryResourceRecord::InitializeFrom(
	const RendererAuxiliaryResourceRecord* p_source,
	LegoU8 p_resourceType
)
{
	m_inUse = 1;
	m_referenceCount = 0;
	memcpy(this, p_source, 0x4c);
	strcpy(m_path, p_source->m_path);
	m_callback = NULL;
	m_resourceType = p_resourceType;
}

// STUB: LEGORACERS2 0x004c05e0
LegoBool32 RendererAuxiliaryResourceRecord::Matches(const RendererAuxiliaryResourceRecord* p_other) const
{
	if (m_inUse == 0 || m_resourceType == 0) {
		return FALSE;
	}
	return memcmp(this, p_other, 0x4c) == 0 && strcmp(m_path, p_other->m_path) == 0;
}

// STUB: LEGORACERS2 0x004c0670
LegoBool32 RendererAuxiliaryResourceRecord::SetCallback(RendererAuxiliaryResourceCallback p_callback)
{
	if (m_path[0] == '\0' || m_callback != NULL) {
		return FALSE;
	}
	m_callback = p_callback;
	return TRUE;
}

// STUB: LEGORACERS2 0x004c0770
LegoS32 DisplayRenderer::AcquireAuxiliaryResource(RendererAuxiliaryResourceRecord* p_record, LegoU32 p_resourceType)
{
	if (p_resourceType == 1) {
		for (LegoS32 i = 0; i < 125; i++) {
			if (m_auxiliaryResources[i].Matches(p_record)) {
				m_auxiliaryResources[i].m_referenceCount++;
				return i;
			}
		}
	}

	LegoS32 index = 0;
	while (m_auxiliaryResources[index].m_path[0] != '\0') {
		index++;
		if (index > 0x7c) {
			return 0;
		}
	}

	RendererAuxiliaryResourceRecord* record = &m_auxiliaryResources[index];
	record->InitializeFrom(p_record, (LegoU8) p_resourceType);
	record->m_referenceCount++;

	if (p_record->m_path[0] != '\0') {
		m_resources0xa0[m_resourceCount0xa4++] = record;
		for (LegoS32 i = 0; i < m_resourceLookupCount; i++) {
			RendererResourceLookupEntry* entry = &m_resourceLookup[i];
			if (strcmp(entry->m_path, record->m_path) == 0) {
				record->SetCallback(entry->m_callback);
				if (record->m_callback == NULL) {
					return index;
				}
				record->m_callback(record, 2);
				return index;
			}
		}
	}

	return index;
}

// STUB: LEGORACERS2 0x004c0b70
LegoS32 DisplayRenderer::CreateTextureResource(
	const LegoChar* p_path,
	LegoU32 p_resourceKind,
	LegoS32 p_resourceIndex,
	LegoU32 p_createMode,
	LegoU16 p_referenceType
)
{
	LegoChar path[0xa4];
	strcpy(path, p_path);
	_strupr(path);
	LegoBool32 loadNow = m_resourceLoadMode != 1;
	LegoU32 pathLength = (LegoU32) strlen(path);
	const LegoChar* suffix = pathLength >= 4 ? path + pathLength - 4 : path;

	if (*suffix != '\0') {
		if (strcmp(suffix, ".TGA") != 0) {
			if (strcmp(suffix, ".IFL") == 0) {
				RendererResourceCacheEntry* cached = FindOrAllocateTextureResource(path, loadNow);
				return AddTextureReference(cached, p_referenceType);
			}
			if (strcmp(suffix, ".AVI") == 0) {
				RendererTextureFactoryDispatch* dispatch = (RendererTextureFactoryDispatch*) m_vtable;
				void* resource = dispatch->m_createExternalTexture(this, NULL, path, loadNow, p_referenceType);
				return AddTextureReference(resource, p_referenceType);
			}
			return -1;
		}
	}

	RendererTextureFactoryDispatch* dispatch = (RendererTextureFactoryDispatch*) m_vtable;
	if (dispatch->m_resourceKindPolicy(this, NULL, p_resourceKind) == 1) {
		if (p_resourceIndex == -1) {
			return -1;
		}
		RendererTextureRecord* record =
			(RendererTextureRecord*) dispatch->m_getTextureRecord(this, NULL, (LegoU32) p_resourceIndex);
		record->m_resourceIndex = (LegoU16) p_resourceKind;
		return AddTextureReference(record, p_referenceType);
	}

	if (dispatch->m_highResolutionPolicy(this, NULL) == 1) {
		char highResolutionPath[0xa4];
		strcpy(highResolutionPath, path);
		LegoChar* slash = strrchr(highResolutionPath, '\\');
		LegoChar* filename = slash != NULL ? slash + 1 : highResolutionPath;
		LegoChar suffixStorage[0xa4];
		strcpy(suffixStorage, filename);
		if (slash != NULL) {
			*(slash + 1) = '\0';
		}
		else {
			highResolutionPath[0] = '\0';
		}
		strcat(highResolutionPath, "HighRes\\");
		strcat(highResolutionPath, suffixStorage);
		LegoU32 highResolutionLength = (LegoU32) strlen(highResolutionPath);
		if (highResolutionLength >= 4) {
			strcpy(highResolutionPath + highResolutionLength - 4, ".MIP");
		}
		if (ManagedPathExists(highResolutionPath)) {
			LegoChar* extension = highResolutionPath + strlen(highResolutionPath) - 4;
			strcpy(extension, ".tga");
		}
		else {
			strcpy(highResolutionPath, path);
		}
		_strupr(highResolutionPath);
		strcpy(path, highResolutionPath);
	}

	RendererTextureRecord* record =
		FindOrCreateTextureRecord(path, p_resourceKind, p_resourceIndex, p_createMode, p_referenceType, loadNow);
	return AddTextureReference(record, p_referenceType);
}

// STUB: LEGORACERS2 0x004c11e0
void RendererAuxiliaryResourceRecord::InitializeMaterial(
	LegoU32 p_mode,
	LegoU32 p_color0,
	LegoU32 p_color1,
	LegoU32 p_color2,
	LegoFloat p_color1Scale,
	LegoFloat p_powerScale,
	LegoFloat p_modeFactor
)
{
	const LegoFloat channelScale = 1.0f / 255.0f;
	const LegoU32 colors[3] = {p_color0, p_color1, p_color2};
	LegoFloat* channels[3] = {m_color0, m_color1, m_color2};
	for (LegoU32 color = 0; color < 3; color++) {
		for (LegoU32 channel = 0; channel < 4; channel++) {
			channels[color][channel] = (LegoFloat) ((colors[color] >> (channel * 8)) & 0xf0) * channelScale;
		}
	}

	m_power = p_powerScale;
	m_modeFactor = p_modeFactor;
	m_mode = p_mode;
	m_color0[3] = 0.0f;
	m_color1[3] = 0.0f;
	m_color2[3] = 0.0f;

	if (m_color1[0] == 0.0f && m_color1[1] == 0.0f && m_color1[2] == 0.0f && m_color1[3] == 0.0f) {
		m_scaledColor1[0] = p_color1Scale;
		m_scaledColor1[1] = p_color1Scale;
		m_scaledColor1[2] = p_color1Scale;
		m_scaledColor1[3] = p_color1Scale;
	}
	else {
		m_scaledColor1[0] = m_color1[0] * p_color1Scale;
		m_scaledColor1[1] = m_color1[1] * p_color1Scale;
		m_scaledColor1[2] = m_color1[2] * p_color1Scale;
		m_scaledColor1[3] = m_color1[3] * p_color1Scale;
	}

	m_color0[0] *= 3.0f;
	m_color0[1] *= 3.0f;
	m_color0[2] *= 3.0f;
	m_color0[3] = 0.0f;
	m_color2[0] *= 0.75f;
	m_color2[1] *= 0.75f;
	m_color2[2] *= 0.75f;
	m_color2[3] = 0.0f;
	m_power *= 192.0f;
	if (m_color2[0] == 0.0f && m_color2[1] == 0.0f && m_color2[2] == 0.0f && m_color2[3] == 0.0f) {
		m_power = 0.0f;
	}
	if (p_modeFactor == 0.0f && p_mode == 1) {
		m_mode = 0;
	}
	m_path[0] = '\0';
	m_callback = NULL;
}
