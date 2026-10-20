#include "render/core/renderer.h"
#include "render/resources/rendererrecords.h"

#include <string.h>

namespace
{
void CopySurfaceRows(IDirect3DSurface8* p_source, IDirect3DSurface8* p_destination, LegoS32 p_height)
{
	D3DLOCKED_RECT sourceLock;
	D3DLOCKED_RECT destinationLock;
	if (p_source->LockRect(&sourceLock, NULL, 0) != D3D_OK) {
		return;
	}
	if (p_destination->LockRect(&destinationLock, NULL, 0) != D3D_OK) {
		p_source->UnlockRect();
		return;
	}

	for (LegoS32 row = p_height - 1; row >= 0; row--) {
		const LegoU8* sourceBytes = (const LegoU8*) sourceLock.pBits + sourceLock.Pitch * row;
		LegoU8* destinationBytes = (LegoU8*) destinationLock.pBits + destinationLock.Pitch * row;
		memcpy(destinationBytes, sourceBytes, (LegoU32) sourceLock.Pitch);
	}
	p_destination->UnlockRect();
	p_source->UnlockRect();
}

void CopyTextureMip(
	IDirect3DBaseTexture8* p_sourceTexture,
	IDirect3DBaseTexture8* p_destinationTexture,
	LegoBool32 p_cube,
	LegoU32 p_face,
	LegoU32 p_sourceLevel,
	LegoU32 p_destinationLevel,
	LegoS32 p_height
)
{
	IDirect3DSurface8* sourceSurface;
	IDirect3DSurface8* destinationSurface;
	if (p_cube) {
		((IDirect3DCubeTexture8*) p_sourceTexture)
			->GetCubeMapSurface((D3DCUBEMAP_FACES) p_face, p_sourceLevel, &sourceSurface);
		((IDirect3DCubeTexture8*) p_destinationTexture)
			->GetCubeMapSurface((D3DCUBEMAP_FACES) p_face, p_destinationLevel, &destinationSurface);
	}
	else {
		((IDirect3DTexture8*) p_sourceTexture)->GetSurfaceLevel(p_sourceLevel, &sourceSurface);
		((IDirect3DTexture8*) p_destinationTexture)->GetSurfaceLevel(p_destinationLevel, &destinationSurface);
	}
	CopySurfaceRows(sourceSurface, destinationSurface, p_height);
	sourceSurface->Release();
	destinationSurface->Release();
}
// Format refresh uses whole-surface byte copies, including compressed blocks.
// Its source faces are separate 2D textures even when the destination is a cube.
void CopyFormatRefreshMip(
	IDirect3DBaseTexture8* p_sourceTexture,
	IDirect3DBaseTexture8* p_destinationTexture,
	LegoBool32 p_destinationCube,
	LegoU32 p_face,
	LegoU32 p_sourceLevel,
	LegoU32 p_destinationLevel
)
{
	IDirect3DSurface8* source;
	IDirect3DSurface8* destination;
	((IDirect3DTexture8*) p_sourceTexture)->GetSurfaceLevel(p_sourceLevel, &source);
	if (p_destinationCube) {
		((IDirect3DCubeTexture8*) p_destinationTexture)
			->GetCubeMapSurface((D3DCUBEMAP_FACES) p_face, p_destinationLevel, &destination);
	}
	else {
		((IDirect3DTexture8*) p_destinationTexture)->GetSurfaceLevel(p_destinationLevel, &destination);
	}
	D3DLOCKED_RECT sourceLock;
	D3DLOCKED_RECT destinationLock;
	source->LockRect(&sourceLock, NULL, 0);
	destination->LockRect(&destinationLock, NULL, 0);
	D3DSURFACE_DESC sourceDescription;
	D3DSURFACE_DESC destinationDescription;
	g_lastDirect3DResult = source->GetDesc(&sourceDescription);
	g_lastDirect3DResult = destination->GetDesc(&destinationDescription);
	if (sourceDescription.Size == destinationDescription.Size) {
		memcpy(destinationLock.pBits, sourceLock.pBits, sourceDescription.Size);
	}
	destination->UnlockRect();
	source->UnlockRect();
	destination->Release();
	source->Release();
}
} // namespace

// STUB: LEGORACERS2 0x004d9610
void RendererTextureRecord::DropTopMip()
{
	const LegoU32 oldFlags = m_flags0x9c;
	if ((oldFlags & 0x20) != 0) {
		return;
	}

	const LegoU16 mipBits = m_flags0x90 & 0xfe00;
	if (mipBits < 0x400) {
		return;
	}

	m_width >>= 1;
	m_height >>= 1;
	m_flags0x90 = (mipBits - 0x200) | (m_flags0x90 & 0x01ff);
	m_flags0x9c = oldFlags | 0x20;

	IDirect3DBaseTexture8* oldPrimary = m_primaryTexture;
	const LegoBool32 cube = (oldFlags & 0x08) != 0 ? TRUE : FALSE;
	GetStaticDisplayRenderer()->CreatePrimaryTexture(this, (oldFlags & 0x10) >> 4);
	if (oldPrimary == NULL) {
		return;
	}

	const LegoU32 levelCount = m_flags0x90 >> 9;
	for (LegoU32 face = 0; face < (cube ? 6u : 1u); face++) {
		for (LegoU32 level = 0; level < levelCount; level++) {
			const LegoS32 height = (LegoS32) m_height >> (level & 0x1f);
			CopyTextureMip(oldPrimary, m_primaryTexture, cube, face, level + 1, level, height);
		}
	}
	oldPrimary->Release();
}

// STUB: LEGORACERS2 0x004dbbe0
void DisplayRenderer::RefreshTextureFormats()
{
	if (m_unk0x4dc != 1) {
		return;
	}

	for (LegoS32 recordIndex = 0; recordIndex < 800; recordIndex++) {
		RendererTextureRecord* record = &m_textureRecords[recordIndex];
		if ((record->m_flags0x06 & 1) == 0 || record->GetFlag0x90() == 0) {
			continue;
		}

		LegoS32 sourceIndex;
		if ((record->m_flags0x9c & 8) != 0) {
			sourceIndex = m_cachedTextureRecordIds[0];
		}
		else {
			if (m_unk0x4f8 == -1) {
				continue;
			}
			sourceIndex = (LegoS32) m_unk0x4f8;
		}
		RendererTextureRecord* formatSource = &m_textureRecords[sourceIndex];

		if (((formatSource->m_flags0x9c ^ record->m_flags0x9c) & 0x003fffc0) != 0) {
			const LegoBool32 hadSecondary = record->m_texture != NULL ? TRUE : FALSE;
			record->ReleaseTexture(0);
			record->ReleaseTexture(1);
			record->m_flags0x9c = (record->m_flags0x9c & 0xffc0003f) | ((formatSource->m_flags0x9c >> 6) & 0xffff) << 6;
			CreatePrimaryTexture(record, TRUE);
			if (hadSecondary) {
				CreateSecondaryTexture(record);
			}
		}

		LegoU32 mipShift = 0;
		for (LegoS16 width = formatSource->m_width; width != record->m_width; width >>= 1) {
			mipShift++;
		}

		if ((record->m_flags0x9c & 8) != 0) {
			for (LegoU32 face = 0; face < 6; face++) {
				RendererTextureRecord* faceSource = &m_textureRecords[m_cachedTextureRecordIds[face]];
				for (LegoS32 level = 0; level < (LegoS32) (record->m_flags0x90 >> 9); level++) {
					CopyFormatRefreshMip(
						faceSource->m_primaryTexture,
						record->m_primaryTexture,
						TRUE,
						face,
						mipShift + level,
						level
					);
				}
			}
		}
		else {
			RendererTextureRecord* textureSource = &m_textureRecords[m_unk0x4f8];
			for (LegoS32 level = 0; level < (LegoS32) (record->m_flags0x90 >> 9); level++) {
				CopyFormatRefreshMip(
					textureSource->m_primaryTexture,
					record->m_primaryTexture,
					FALSE,
					0,
					mipShift + level,
					level
				);
			}
		}

		if (record->m_texture != NULL) {
			record->UpdateGpuTexture();
		}
	}
}

// STUB: LEGORACERS2 0x004dbfb0
void RendererTextureRecord::ReleaseTexture(LegoU32 p_slot)
{
	IDirect3DBaseTexture8** textureSlot = (IDirect3DBaseTexture8**) (&m_primaryTexture + p_slot);
	if (*textureSlot != NULL) {
		(*textureSlot)->Release();
		*textureSlot = NULL;
	}
}
