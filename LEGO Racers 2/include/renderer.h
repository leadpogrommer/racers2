#ifndef LR2_RENDERER_H
#define LR2_RENDERER_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

struct RendererAdapterRecord {
	LegoU32 m_adapterIndex;
	LegoChar m_driverName[512];
	LegoChar m_description[512];
	D3DCAPS8 m_caps;
};

DECOMP_SIZE_ASSERT(RendererAdapterRecord, 0x4d8);

// This is an evidenced prefix of the static renderer object at 0x0058b140.
// The full object extends beyond this prefix; its complete size is not known.
class DisplayRenderer {
public:
	LegoBool32 EnumerateSupportedAdapters();
	void ConfigureNvidiaWorkaround();
	LegoBool32 SelectDepthStencilFormat();
	LegoBool32 FindBestDisplayMode(LegoU32* p_width, LegoU32* p_height, LegoU32* p_bitDepth, LegoU32* p_refreshRate);
	LegoU32 GetSelectedAdapterModeCount();
	void GetSelectedAdapterMode(
		LegoU32 p_modeIndex,
		LegoU32* p_width,
		LegoU32* p_height,
		LegoU32* p_bitDepth,
		LegoU32* p_refreshRate
	);

	void* m_vtable; // 0x000
	undefined m_unk0x4[0x7ac - 0x4];
	IDirect3D8* m_direct3D;          // 0x7ac
	IDirect3DDevice8* m_device;      // 0x7b0
	LegoU32 m_requestedAdapterIndex; // 0x7b4
	undefined m_unk0x7b8[0x7dc - 0x7b8];
	D3DFORMAT m_depthStencilFormat; // 0x7dc
	undefined m_unk0x7e0[0x7f8 - 0x7e0];
	LegoU32 m_modeWidth;         // 0x7f8
	LegoU32 m_modeHeight;        // 0x7fc
	LegoU32 m_modeRefreshRate;   // 0x800
	D3DFORMAT m_modeFormat;      // 0x804, raw format rather than derived bit depth
	LegoU32 m_requestedBitDepth; // 0x808
	undefined m_unk0x80c[0x820 - 0x80c];
	RendererAdapterRecord* m_selectedAdapter; // 0x820
	undefined m_unk0x824[0x864 - 0x824];
	RendererAdapterRecord m_adapters[10]; // 0x864
	LegoU32 m_adapterCount;               // 0x38d4
	undefined m_unk0x38d8[0x2456c - 0x38d8];
	LegoU32 m_unk0x2456c;                   // 0x2456c, cleared before the NVIDIA config gate
	LegoBool32 m_foundPreferredDepthFormat; // 0x24570, selected D24S8
	LegoU32 m_unk0x24574;                   // 0x24574
	undefined m_unk0x24578[0xc7ec4 - 0x24578];
	LegoBool32 m_nvidiaWorkaround; // 0xc7ec4
};

// This remains a prefix: the constructor proves storage through +0xc7f2b.
DECOMP_SIZE_ASSERT(DisplayRenderer, 0xc7ec8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_direct3D) == 0x7ac);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_depthStencilFormat) == 0x7dc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_modeFormat) == 0x804);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedAdapter) == 0x820);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_adapterCount) == 0x38d4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_unk0x2456c) == 0x2456c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_foundPreferredDepthFormat) == 0x24570);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_nvidiaWorkaround) == 0xc7ec4);

extern HRESULT g_lastDirect3DResult;

LegoU32 __stdcall GetDisplayFormatBitDepth(D3DFORMAT p_format);

#endif // LR2_RENDERER_H
