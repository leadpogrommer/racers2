#include "applicationpolicy.h"
#include "bitmapimage.h"
#include "renderer.h"
#include "rendererrecords.h"

#include <string.h>

LegoBool32 WINAPI AcceptApplicationMessage(MSG*);
void ApplicationNoOp();
void __fastcall ReleaseRendererTextureResource(RendererTextureRecord*, void*);
LegoBool32 __fastcall EnsureMipSidecar(RendererTextureRecord*, void*, const LegoChar*, BitmapImage*);

typedef LegoBool32(__fastcall* RendererPathPolicy)(DisplayRenderer*, void*);
typedef void(__fastcall* RendererPathAction)(DisplayRenderer*, void*, const LegoChar*);
typedef void(__fastcall* RendererTexturePathAction)(DisplayRenderer*, void*, const LegoChar*, BitmapImage*);
typedef void(__fastcall*
				 RendererPathImageAction)(DisplayRenderer*, void*, const LegoChar*, const LegoChar*, BitmapImage*);
typedef void(__fastcall*
				 RendererTextureImageAction)(DisplayRenderer*, void*, LegoS32, BitmapImage*, LegoS32, const LegoChar*);
typedef void(__fastcall*
				 RendererCubeFaceAction)(DisplayRenderer*, void*, LegoS32, BitmapImage*, const LegoChar*, LegoS32);
typedef void(__fastcall* RendererReleaseTextureRecord)(DisplayRenderer*, void*, LegoS32);

struct DisplayRendererTextureLoadDispatch {
	undefined m_unk0x000[0xd8];
	RendererPathImageAction m_buildMipImage;        // 0xd8
	RendererTexturePathAction m_prepareTexturePath; // 0xdc
	RendererTextureImageAction m_loadTextureImage;  // 0xe0
	undefined m_unk0xe4[0x04];
	RendererPathPolicy m_useHighResolutionTextures; // 0xe8
	RendererCubeFaceAction m_loadCubeFace;          // 0xec
};

struct DisplayRendererTextureReleaseDispatch {
	undefined m_unk0x000[0xe4];
	RendererReleaseTextureRecord m_releaseTextureRecord; // 0xe4
};

typedef LegoBool32(__fastcall* Message)(RendererTextureRecord*, void*, MSG*);
typedef LegoBool32(__fastcall* RecordAction)(RendererTextureRecord*, void*);
typedef void(__fastcall* RecordNoOp)(RendererTextureRecord*, void*);
typedef void(__fastcall* RecordDiagnostic)(RendererTextureRecord*, void*, const LegoChar*);
typedef LegoU32(__fastcall* RecordValue)(RendererTextureRecord*, void*);
typedef LegoBool32(__fastcall* RecordLock)(RendererTextureRecord*, void*, LegoS32*, LegoU32, void**);
typedef void(__fastcall* RecordUnlock)(RendererTextureRecord*, void*, LegoU32);
typedef void(__fastcall* RecordUpload)(RendererTextureRecord*, void*, BitmapImage*);

struct RendererTextureRecordDispatchTable {
	Message m_acceptMessage;        // 0x00, 0049a640
	RecordAction m_load;            // 0x04, 004bfd70
	RecordNoOp m_noOp;              // 0x08, 004038d0
	RecordNoOp m_release;           // 0x0c, 004c0050
	RecordAction m_developmentMode; // 0x10, 00403910
	RecordDiagnostic m_diagnostic;  // 0x14, 00403930
	RecordValue m_flag90;           // 0x18, 004abe80
	RecordLock m_lockSurface;       // 0x1c, 004da490
	RecordUnlock m_unlockSurface;   // 0x20, 004da500
	RecordUpload m_uploadImage;     // 0x24, 004da540
};
DECOMP_SIZE_ASSERT(RendererTextureRecordDispatchTable, 0x28);

LegoBool32 __fastcall AcceptTextureRecordMessage(RendererTextureRecord*, void*, MSG* p_message)
{
	return AcceptApplicationMessage(p_message);
}

LegoBool32 __fastcall LoadTextureRecord(RendererTextureRecord* p_record, void*)
{
	return p_record->Load();
}

void __fastcall TextureRecordNoOp(RendererTextureRecord*, void*)
{
	ApplicationNoOp();
}

LegoBool32 __fastcall TextureRecordDevelopmentMode(RendererTextureRecord*, void*)
{
	return IsApplicationDevelopmentMode();
}

void __fastcall TextureRecordDiagnostic(RendererTextureRecord*, void*, const LegoChar* p_message)
{
	OnApplicationDiagnostic(p_message);
}

LegoU32 __fastcall TextureRecordGetFlag90(RendererTextureRecord* p_record, void*)
{
	return p_record->GetFlag0x90();
}

LegoBool32 __fastcall TextureRecordLockSurface(
	RendererTextureRecord* p_record,
	void*,
	LegoS32* p_pitch,
	LegoU32 p_ignored,
	void** p_bits
)
{
	return p_record->LockPrimarySurface(p_pitch, p_ignored, p_bits);
}

void __fastcall TextureRecordUnlockSurface(RendererTextureRecord* p_record, void*, LegoU32 p_ignored)
{
	p_record->UnlockPrimarySurface(p_ignored);
}

void __fastcall TextureRecordUploadImage(RendererTextureRecord* p_record, void*, BitmapImage* p_image)
{
	p_record->UploadBitmapImage(p_image);
}

namespace
{
void SetMipSuffix(LegoChar* p_path, const LegoChar* p_replacement)
{
	strcpy(p_path + strlen(p_path) - 4, p_replacement);
}

} // namespace

// Retail explicit dispatch table: 0x005358e4 (not a native C++ vftable).
extern const RendererTextureRecordDispatchTable g_rendererTextureRecordDispatchTable = {
	AcceptTextureRecordMessage,
	LoadTextureRecord,
	TextureRecordNoOp,
	ReleaseRendererTextureResource,
	TextureRecordDevelopmentMode,
	TextureRecordDiagnostic,
	TextureRecordGetFlag90,
	TextureRecordLockSurface,
	TextureRecordUnlockSurface,
	TextureRecordUploadImage
};

// STUB: LEGORACERS2 0x004abe50
RendererTextureRecord::RendererTextureRecord()
{
	m_flags0x06 &= 0xc000;
	m_flags0x9c &= 0xfffffff1;
	m_vtable = (void*) &g_rendererTextureRecordDispatchTable;
	m_primaryTexture = NULL;
	m_texture = NULL;
}

// STUB: LEGORACERS2 0x004bf920
LegoBool32 __fastcall LoadMipSidecar(
	RendererTextureRecord* p_record,
	void*,
	const LegoChar* p_path,
	BitmapImage* p_image
)
{
	LegoChar mipPath[136];
	strcpy(mipPath, p_path);
	SetMipSuffix(mipPath, ".TGA");
	LegoChar* const originalSuffix = mipPath + strlen(mipPath) - 4;
	strcat(mipPath, ".MIP");

	if (p_image->LoadTga(mipPath, 0) != FALSE) {
		return TRUE;
	}
	if (p_image->LoadTga(p_path, 0) == FALSE) {
		return FALSE;
	}

	strcpy(originalSuffix, "_NM.TGA");
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	DisplayRendererTextureLoadDispatch* dispatch = (DisplayRendererTextureLoadDispatch*) renderer->m_vtable;
	dispatch->m_prepareTexturePath(renderer, NULL, mipPath, p_image);
	p_image->ReleaseStorage();
	return EnsureMipSidecar(p_record, NULL, mipPath, p_image);
}

// STUB: LEGORACERS2 0x004bfa60
LegoBool32 __fastcall EnsureMipSidecar(RendererTextureRecord*, void*, const LegoChar* p_path, BitmapImage* p_image)
{
	LegoChar mipPath[132];
	strcpy(mipPath, p_path);
	strcat(mipPath, ".MIP");

	if (p_image->LoadTga(mipPath, 0) != FALSE) {
		return TRUE;
	}
	if (p_image->LoadTga(p_path, 0) == FALSE) {
		return FALSE;
	}

	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	DisplayRendererTextureLoadDispatch* dispatch = (DisplayRendererTextureLoadDispatch*) renderer->m_vtable;
	dispatch->m_buildMipImage(renderer, NULL, p_path, ".MIP", p_image);
	p_image->ReleaseStorage();
	return p_image->LoadTga(mipPath, 0);
}

// STUB: LEGORACERS2 0x004bfd70
LegoBool32 RendererTextureRecord::Load()
{
	m_flags0x06 &= 0xfffd;
	if (m_resourcePath[0] == '\0') {
		return TRUE;
	}

	BitmapImage image;
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	DisplayRendererTextureLoadDispatch* dispatch = (DisplayRendererTextureLoadDispatch*) renderer->m_vtable;
	if (m_resourceType == 9) {
		const LegoChar* filename = strrchr(m_resourcePath, '\\');
		filename = filename != NULL ? filename + 1 : m_resourcePath;
		if (strcmp(filename, "DEFAULT SKY BOX.TGA") == 0) {
			m_flags0x90 = (m_flags0x90 & 0xfffd) | 2;
		}
	}

	if (m_resourceType == 9 && dispatch->m_useHighResolutionTextures(renderer, NULL) == TRUE) {
		static const LegoChar* const faces[6] = {"_RT.TGA", "_LF.TGA", "_DN.TGA", "_UP.TGA", "_BK.TGA", "_FR.TGA"};
		LegoChar facePath[132];
		strcpy(facePath, m_resourcePath);
		LegoS32 faceIndex;
		for (faceIndex = 0; faceIndex < 6; faceIndex++) {
			strcpy(facePath + strlen(m_resourcePath), faces[faceIndex]);
			if (EnsureMipSidecar(this, NULL, facePath, &image) == FALSE) {
				image.ReleaseStorage();
				return FALSE;
			}
			dispatch->m_loadCubeFace(renderer, NULL, m_resourceId, &image, facePath, faceIndex);
			image.ReleaseStorage();
		}
	}
	else {
		LegoBool32 loaded;
		if (m_resourceType == 3) {
			loaded = LoadMipSidecar(this, NULL, m_resourcePath, &image);
		}
		else {
			loaded = EnsureMipSidecar(this, NULL, m_resourcePath, &image);
		}
		if (loaded == FALSE) {
			image.ReleaseStorage();
			return FALSE;
		}
		dispatch->m_loadTextureImage(renderer, NULL, m_resourceId, &image, (LegoS16) m_resourceType, m_resourcePath);
		image.ReleaseStorage();
	}

	image.ReleaseStorage();
	return TRUE;
}

// STUB: LEGORACERS2 0x004c0050
void __fastcall ReleaseRendererTextureResource(RendererTextureRecord* p_record, void*)
{
	DisplayRenderer* renderer = g_displayRenderer;
	DisplayRendererTextureReleaseDispatch* dispatch = (DisplayRendererTextureReleaseDispatch*) renderer->m_vtable;
	dispatch->m_releaseTextureRecord(renderer, NULL, p_record->m_resourceId);
}

// STUB: LEGORACERS2 0x004da1e0
void DisplayRenderer::ReleaseTextureRecord(LegoS32 p_recordIndex)
{
	if (p_recordIndex == -1) {
		return;
	}

	RendererTextureRecord* record = &m_textureRecords[p_recordIndex];
	IDirect3DBaseTexture8** texture = &record->m_primaryTexture;
	for (LegoU32 i = 0; i < 2; i++) {
		if (texture[i] != NULL) {
			if ((record->m_flags0x9c & 8) != 0) {
				texture[i]->Release();
			}
			else {
				texture[i]->Release();
			}
			texture[i] = NULL;
		}
	}
}
