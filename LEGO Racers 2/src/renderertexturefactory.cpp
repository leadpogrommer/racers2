#include "renderer.h"
#include "rendererrecords.h"

#include <stddef.h>

namespace
{
typedef void(__fastcall* SetTextureIndexMap)(DisplayRenderer*, void*, LegoU32, LegoU32);

struct DisplayRendererTextureFactoryDispatch {
	undefined m_unk0x000[0x11c];
	SetTextureIndexMap m_setTextureIndexMap;
};

typedef void(__fastcall* ReleaseTextureRecord)(RendererTextureRecord*, void*);

struct RendererTextureRecordReleaseDispatch {
	undefined m_unk0x00[0x0c];
	ReleaseTextureRecord m_release;
};
} // namespace

// STUB: LEGORACERS2 0x004d9f20
LegoBool32 DisplayRenderer::CreateTextureRecord(
	LegoU32 p_createMode,
	LegoS32 p_recordIndex,
	LegoU32 p_width,
	LegoU32 p_height,
	LegoU32 p_mipLevels,
	LegoU32* p_compressedOutput,
	LegoU32 p_flagA
)
{
	DisplayRendererTextureFactoryDispatch* dispatch = (DisplayRendererTextureFactoryDispatch*) m_vtable;
	dispatch->m_setTextureIndexMap(this, NULL, (LegoU32) p_recordIndex, (LegoU32) p_recordIndex);

	RendererTextureRecord* record = &m_textureRecords[p_recordIndex];
	if (!SetTextureRecordDimensions(record, p_width, p_height, p_mipLevels, p_flagA, 0, 0)) {
		return FALSE;
	}

	*p_compressedOutput = 0;
	if (p_createMode == 0) {
		record->m_flags0x9c = (record->m_flags0x9c & 0xffc001ff) | 0x1c0;
	}
	else if (p_createMode == 1) {
		record->m_flags0x9c = (record->m_flags0x9c & 0xffc001ff) | 0x1c0;
		const LegoU32 formatIndex = (record->m_flags0x9c >> 6) & 0xffff;
		RendererTextureFormat* format = m_selectedTextureFormats[formatIndex];
		if (format->m_format == D3DFMT_A8R8G8B8) {
			*p_compressedOutput = 1;
		}
	}
	else if (p_createMode == 2) {
		record->m_flags0x9c = (record->m_flags0x9c & 0xffc000ff) | 0xc0;
	}

	if (!CreatePrimaryTexture(record, FALSE)) {
		RendererTextureRecordReleaseDispatch* recordDispatch = (RendererTextureRecordReleaseDispatch*) record->m_vtable;
		recordDispatch->m_release(record, NULL);
		record->m_flags0x06 &= 0xfffe;
		return FALSE;
	}

	if (m_createDeviceBuffers == 1) {
		CreateSecondaryTexture(record);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004da490
LegoBool32 RendererTextureRecord::LockPrimarySurface(LegoS32* p_pitch, LegoU32 p_ignored, void** p_bits)
{
	IDirect3DSurface8* surface = NULL;
	((IDirect3DTexture8*) m_primaryTexture)->GetSurfaceLevel(0, &surface);

	D3DLOCKED_RECT locked;
	if (surface->LockRect(&locked, NULL, 0) != D3D_OK) {
		surface->Release();
		return FALSE;
	}
	*p_pitch = locked.Pitch;
	*p_bits = locked.pBits;
	surface->Release();
	return TRUE;
}

// STUB: LEGORACERS2 0x004da500
void RendererTextureRecord::UnlockPrimarySurface(LegoU32 p_ignored)
{
	IDirect3DSurface8* surface = NULL;
	((IDirect3DTexture8*) m_primaryTexture)->GetSurfaceLevel(0, &surface);
	surface->UnlockRect();
	surface->Release();
	UpdateGpuTexture();
}
