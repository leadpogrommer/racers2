#ifndef LR2_RENDERER_H
#define LR2_RENDERER_H

#include "decomp.h"
#include "directx8.h"
#include "pixelformat.h"
#include "rendererdispatch.h"
#include "types.h"

#include <stddef.h>

struct RendererAdapterRecord {
	LegoU32 m_adapterIndex;
	LegoChar m_driverName[512];
	LegoChar m_description[512];
	D3DCAPS8 m_caps;
};

DECOMP_SIZE_ASSERT(RendererAdapterRecord, 0x4d8);

struct RendererTextureFormat {
	D3DFORMAT m_format;
	LegoU32 m_selectedAsCompressed; // 0x04, set when a DXT preference is selected
	PixelFormatDescription m_description;
};

DECOMP_SIZE_ASSERT(RendererTextureFormat, 0x28);

// This is an evidenced prefix of the static renderer object at 0x0058b140.
// The full object extends beyond this prefix; its complete size is not known.
class DisplayRenderer {
public:
	void SetDepthState(LegoU32 p_enabled);
	void SetAlphaBlendMode(LegoU32 p_mode);
	void SetMaterialColor(LegoU32 p_color);
	LegoBool32 ClearBuffers(LegoU32 p_flags);
	LegoBool32 Present();
	LegoS32 GetTextureQuality();
	void AddSupportedTextureFormat(D3DFORMAT p_format);
	LegoBool32 InitializeTextureFormats();
	LegoS32 SelectTextureFormat(LegoU32 p_flags, LegoU32 p_compressed, LegoU32 p_force32Bit);
	LegoBool32 EnumerateSupportedAdapters();
	void ConfigureNvidiaWorkaround();
	LegoBool32 SelectDepthStencilFormat();
	LegoBool32 UsesHardwareVertexProcessing();
	LegoBool32 CreateVertexBuffers();
	void InitializeDefaultRenderStates();
	void SetCachedRenderState(D3DRENDERSTATETYPE p_state, LegoU32 p_value);
	void ResetTextureStages();
	void SetCachedTextureStageState(LegoU32 p_stage, D3DTEXTURESTAGESTATETYPE p_state, LegoU32 p_value);
	void SetTextureFiltering(LegoU32 p_linear);
	LegoBool32 FindBestDisplayMode(LegoU32* p_width, LegoU32* p_height, LegoU32* p_bitDepth, LegoU32* p_refreshRate);
	LegoU32 GetSelectedAdapterModeCount();
	void GetSelectedAdapterMode(
		LegoU32 p_modeIndex,
		LegoU32* p_width,
		LegoU32* p_height,
		LegoU32* p_bitDepth,
		LegoU32* p_refreshRate
	);

	const RendererDispatchView* m_vtable; // 0x000
	undefined m_unk0x4[0x4d8 - 0x4];
	LegoS32 m_unk0x4d8;
	undefined m_unk0x4dc[0x780 - 0x4dc];
	LegoFloat m_clearQuadDepth; // 0x780
	undefined m_unk0x784[0x7ac - 0x784];
	IDirect3D8* m_direct3D;                         // 0x7ac
	IDirect3DDevice8* m_device;                     // 0x7b0
	LegoU32 m_requestedAdapterIndex;                // 0x7b4
	D3DPRESENT_PARAMETERS m_presentationParameters; // 0x7b8..0x7eb
	LegoS32 m_textureQuality;                       // 0x7ec
	LegoU32 m_deviceBehaviorFlags;                  // 0x7f0, passed to IDirect3D8::CreateDevice
	undefined m_unk0x7f4[0x7f8 - 0x7f4];
	LegoU32 m_modeWidth;           // 0x7f8
	LegoU32 m_modeHeight;          // 0x7fc
	LegoU32 m_modeRefreshRate;     // 0x800
	D3DFORMAT m_modeFormat;        // 0x804, raw format rather than derived bit depth
	LegoU32 m_requestedBitDepth;   // 0x808
	LegoBool32 m_clockwiseCulling; // 0x80c, chooses D3DCULL_CW rather than CCW
	undefined m_unk0x810[0x820 - 0x810];
	RendererAdapterRecord* m_selectedAdapter;                     // 0x820
	RendererTextureFormat* m_selectedTextureFormats[8];           // 0x824
	RendererTextureFormat* m_selectedCompressedTextureFormats[8]; // 0x844
	RendererAdapterRecord m_adapters[10];                         // 0x864
	LegoU32 m_adapterCount;                                       // 0x38d4
	undefined m_unk0x38d8[0x2395c - 0x38d8];
	LegoS32 m_textureFormatCount;               // 0x2395c
	RendererTextureFormat m_textureFormats[64]; // 0x23960
	undefined m_unk0x24360[0x24520 - 0x24360];
	LegoU32 m_clearRenderTarget; // 0x24520
	undefined m_unk0x24524[0x2456c - 0x24524];
	LegoU32 m_vertexBufferUsage;            // 0x2456c, D3DUSAGE_WRITEONLY or zero
	LegoBool32 m_foundPreferredDepthFormat; // 0x24570, selected D24S8
	LegoU32 m_unk0x24574;                   // 0x24574
	undefined m_unk0x24578[0x24598 - 0x24578];
	LegoU32 m_ambientColor; // 0x24598, D3DRS_AMBIENT default
	undefined m_unk0x2459c[4];
	D3DMATERIAL8 m_material; // 0x245a0
	undefined m_unk0x245e4[0xc626c - 0x245e4];
	LegoU32 m_fillMode; // 0xc626c
	undefined m_unk0xc6270[0xc6288 - 0xc6270];
	LegoU32 m_useBlendModeOverride; // 0xc6288
	LegoU32 m_blendModeOverride;    // 0xc628c
	undefined m_unk0xc6290[0xc734c - 0xc6290];
	LegoU32 m_cachedRenderStates[0xac];      // 0xc734c
	LegoU32 m_cachedTextureHandles[8];       // 0xc75fc
	LegoU32 m_unk0xc761c;                    // 0xc761c
	LegoU32 m_unk0xc7620;                    // 0xc7620
	LegoU32 m_cachedTextureStageStates[256]; // 0xc7624, (stage << 5) | state
	undefined m_unk0xc7a24[0xc7ec4 - 0xc7a24];
	LegoBool32 m_nvidiaWorkaround; // 0xc7ec4
	undefined m_unk0xc7ec8[0xc7edc - 0xc7ec8];
	LegoU32 m_vertexBufferCapacity;                    // 0xc7edc
	IDirect3DVertexBuffer8* m_normalVertexBuffer;      // 0xc7ee0, FVF 0x152
	IDirect3DVertexBuffer8* m_transformedVertexBuffer; // 0xc7ee4, FVF 0x144
	undefined m_unk0xc7ee8[0xc7ef8 - 0xc7ee8];
	IDirect3DVertexBuffer8* m_specularVertexBuffer; // 0xc7ef8, FVF 0x1c4
};

// This remains a prefix: the constructor proves storage through +0xc7f2b.
DECOMP_SIZE_ASSERT(DisplayRenderer, 0xc7efc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_direct3D) == 0x7ac);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_clearQuadDepth) == 0x780);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureQuality) == 0x7ec);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureFormatCount) == 0x2395c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_textureFormats) == 0x23960);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_clearRenderTarget) == 0x24520);
DECOMP_SIZE_ASSERT(D3DMATERIAL8, 0x44);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_material) == 0x245a0);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_useBlendModeOverride) == 0xc6288);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_blendModeOverride) == 0xc628c);
DECOMP_SIZE_ASSERT(D3DPRESENT_PARAMETERS, 0x34);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_presentationParameters) == 0x7b8);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_modeFormat) == 0x804);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedAdapter) == 0x820);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedTextureFormats) == 0x824);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_selectedCompressedTextureFormats) == 0x844);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_adapterCount) == 0x38d4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_deviceBehaviorFlags) == 0x7f0);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_vertexBufferUsage) == 0x2456c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_foundPreferredDepthFormat) == 0x24570);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_ambientColor) == 0x24598);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_fillMode) == 0xc626c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_cachedRenderStates) == 0xc734c);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_cachedTextureHandles) == 0xc75fc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_cachedTextureStageStates) == 0xc7624);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_nvidiaWorkaround) == 0xc7ec4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_vertexBufferCapacity) == 0xc7edc);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_normalVertexBuffer) == 0xc7ee0);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_transformedVertexBuffer) == 0xc7ee4);
DECOMP_STATIC_ASSERT(offsetof(DisplayRenderer, m_specularVertexBuffer) == 0xc7ef8);

extern HRESULT g_lastDirect3DResult;

LegoU32 __stdcall GetDisplayFormatBitDepth(D3DFORMAT p_format);

#endif // LR2_RENDERER_H
