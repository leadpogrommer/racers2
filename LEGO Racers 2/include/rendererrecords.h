#ifndef LR2_RENDERERRECORDS_H
#define LR2_RENDERERRECORDS_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

class BitmapImage;

struct RendererTextureRecord {
	void* m_vtable;
	LegoS16 m_resourceId;
	LegoU16 m_flags0x06;
	LegoChar m_resourcePath[0x80];
	LegoS16 m_width;
	LegoS16 m_height;
	LegoU16 m_resourceType;
	LegoU16 m_resourceIndex;
	LegoU16 m_flags0x90;
	undefined m_unk0x92[0x94 - 0x92];
	IDirect3DBaseTexture8* m_primaryTexture;
	IDirect3DBaseTexture8* m_texture;
	LegoU32 m_flags0x9c;

	RendererTextureRecord();
	LegoBool32 Load();
	LegoU32 GetFlag0x90();
	void SetResourcePath(const LegoChar* p_path, LegoU16 p_type, LegoU8 p_enabled);
	LegoBool32 MatchesResource(const LegoChar* p_path, LegoU32 p_pathHash, LegoS32 p_resourceType) const;
	void DropTopMip();
	void ReleaseTexture(LegoU32 p_slot);
	void UploadBitmapImage(BitmapImage* p_image);
	void UpdateGpuTexture();
	LegoBool32 LockPrimarySurface(LegoS32* p_pitch, LegoU32 p_ignored, void** p_bits);
	void UnlockPrimarySurface(LegoU32 p_ignored);
};

DECOMP_SIZE_ASSERT(RendererTextureRecord, 0xa0);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_resourceId) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_flags0x06) == 0x06);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_resourcePath) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_width) == 0x88);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_height) == 0x8a);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_resourceType) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_resourceIndex) == 0x8e);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_flags0x90) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_primaryTexture) == 0x94);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_texture) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(RendererTextureRecord, m_flags0x9c) == 0x9c);

// The renderer's secondary resource cache stores references separately from
// the fixed 800-entry texture-record table.
struct RendererResourceRef {
	void* m_resource;
	LegoU16 m_referenceCount;
	LegoU16 m_resourceType;
};

DECOMP_SIZE_ASSERT(RendererResourceRef, 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceRef, m_referenceCount) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceRef, m_resourceType) == 0x06);

struct RendererAuxiliaryResourceRecord {
	RendererAuxiliaryResourceRecord* InitializeHeader();
	void CopyToDeviceMaterial(D3DMATERIAL8* p_material) const;
	LegoFloat m_color0[4];
	LegoFloat m_color1[4];
	LegoFloat m_color2[4];
	LegoFloat m_scaledColor1[4];
	LegoFloat m_power;
	LegoFloat m_modeFactor;
	LegoU32 m_mode;
	LegoChar m_path[8];
	void* m_callback;
	LegoU8 m_inUse;
	LegoU8 m_resourceType;
	LegoU16 m_referenceCount;

	void InitializeMaterial(
		LegoU32 p_mode,
		LegoU32 p_color0,
		LegoU32 p_color1,
		LegoU32 p_color2,
		LegoFloat p_color1Scale,
		LegoFloat p_powerScale,
		LegoFloat p_modeFactor
	);
	void InitializeFrom(const RendererAuxiliaryResourceRecord* p_source, LegoU8 p_resourceType);
	LegoBool32 Matches(const RendererAuxiliaryResourceRecord* p_other) const;
	LegoBool32 SetCallback(void* p_callback);
};

DECOMP_SIZE_ASSERT(RendererAuxiliaryResourceRecord, 0x5c);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_color1) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_color2) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_scaledColor1) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_power) == 0x40);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_modeFactor) == 0x44);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_mode) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_path) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_callback) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_inUse) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(RendererAuxiliaryResourceRecord, m_referenceCount) == 0x5a);

struct RendererResourceLookupEntry {
	LegoU32 m_resource;
	LegoChar m_path[8];
};

DECOMP_SIZE_ASSERT(RendererResourceLookupEntry, 0x0c);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceLookupEntry, m_path) == 0x04);

// Observed stride is 0x194. Only the callback and lookup prefix are named;
// the remaining storage is preserved as opaque bytes until its full layout is
// recovered.
struct RendererResourceCacheEntry {
	RendererResourceCacheEntry(LegoU8 p_type);
	void InitializeRecord();
	LegoBool32 SetPath(const LegoChar* p_path);
	LegoBool32 LoadAnimation();
	void AdvanceFrame();
	void ReleaseAnimationReferences();

	void* m_vtable;
	LegoS16 m_resourceId;
	LegoU16 m_flags0x06;
	LegoChar m_path[128];
	LegoS16 m_frameResourceIds[64]; // 0x88
	LegoS16 m_frameEndTimes[64];    // 0x108
	LegoU32 m_frameCount;           // 0x188
	LegoU32 m_elapsedTicks;         // 0x18c
	LegoU32 m_totalDuration;        // 0x190
};

DECOMP_SIZE_ASSERT(RendererResourceCacheEntry, 0x194);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_flags0x06) == 0x06);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_path) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_frameResourceIds) == 0x88);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_frameEndTimes) == 0x108);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_frameCount) == 0x188);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_elapsedTicks) == 0x18c);
DECOMP_STATIC_ASSERT(offsetof(RendererResourceCacheEntry, m_totalDuration) == 0x190);

struct RendererDefaultSlot {
	LegoU32 m_value;
	LegoU16 m_mask;
	LegoU8 m_kind;
	LegoU8 m_flags;
};

DECOMP_SIZE_ASSERT(RendererDefaultSlot, 0x08);

// A plain defaults record initialized by the renderer constructor at +0x28740.
// Only its written fields and the four 8-byte default slots are modeled.
struct RendererDefaultRecord {
	LegoU32 m_unk0x00;
	LegoU16 m_unk0x04;
	LegoU8 m_unk0x06;
	LegoU8 m_unk0x07;
	RendererDefaultSlot m_slots[4];

	RendererDefaultRecord();
};

DECOMP_SIZE_ASSERT(RendererDefaultRecord, 0x28);
DECOMP_STATIC_ASSERT(offsetof(RendererDefaultRecord, m_slots) == 0x08);

// The renderer constructor applies this first-word default to 256 elements
// with a 0x14-byte stride; trailing bytes are left untouched.
struct RendererArrayMarker {
	LegoU32 m_value;
	undefined m_unk0x04[0x14 - 0x04];

	RendererArrayMarker();
};

DECOMP_SIZE_ASSERT(RendererArrayMarker, 0x14);

LegoU32 GetTexturePathHash(const LegoChar* p_path);

#endif // LR2_RENDERERRECORDS_H
