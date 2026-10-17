#ifndef LR2_RENDERERDISPATCH_H
#define LR2_RENDERERDISPATCH_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

class DisplayRenderer;
class RendererIndexBuffer;
struct RendererTextureRecord;

// View of an observed game dispatch slot. This does not construct a table or
// propose a source class hierarchy. The preceding slots retain their offsets.
struct RendererDispatchView {
	typedef LegoBool32(__fastcall* Policy)(DisplayRenderer*, void*);
	typedef void(__fastcall* SetValue)(DisplayRenderer*, void*, LegoU32);
	typedef void(__fastcall* ThreeValues)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* UpdateViewport)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoU32, LegoU32);
	typedef LegoBool32(__fastcall* SetPolicy)(DisplayRenderer*, void*, LegoU32);
	typedef LegoS32(__fastcall* AddViewport)(DisplayRenderer*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
	typedef RendererIndexBuffer*(__fastcall* CreateIndexBuffer)(DisplayRenderer*, void*, LegoU32, LegoS32);
	typedef RendererTextureRecord*(__fastcall* GetTextureRecord)(DisplayRenderer*, void*, LegoS32);
	typedef LegoS32(__fastcall* SignedValue)(DisplayRenderer*, void*);
	typedef void(__fastcall* SetPointer)(DisplayRenderer*, void*, const void*);
	undefined m_unk0x0[0x1c];
	SetValue m_setMaterialColor; // 0x1c
	SetPolicy m_clearBuffers;    // 0x20
	Policy m_present;            // 0x24
	undefined m_unk0x28[0x34 - 0x28];
	typedef void(__fastcall* Action)(DisplayRenderer*, void*);
	Action m_uploadDefaultVertexConstants; // 0x34
	undefined m_unk0x38[0x8c - 0x38];
	SetValue m_action0x8c; // 0x8c, target4dee90
	undefined m_unk0x90[0x94 - 0x90];
	ThreeValues m_setStencilOperation; // 0x94, target4deff0
	SetValue m_setDepthState;          // 0x98
	undefined m_unk0x9c[0xb0 - 0x9c];
	AddViewport m_addViewport;       // 0xb0
	UpdateViewport m_updateViewport; // 0xb4
	SetValue m_setRenderTarget;      // 0xb8
	undefined m_unk0xbc[0xc8 - 0xbc];
	SetValue m_action0xc8; // 0xc8
	undefined m_unk0xcc[0x100 - 0xcc];
	CreateIndexBuffer m_createIndexBuffer; // 0x100
	undefined m_unk0x104[0x114 - 0x104];
	GetTextureRecord m_getTextureRecord; // 0x114
	undefined m_unk0x118[0x124 - 0x118];
	SetValue m_action0x124; // 0x124
	undefined m_unk0x128[0x140 - 0x128];
	SetPointer m_drawQuad; // 0x140, caller supplies a quad packet
	undefined m_unk0x144[0x184 - 0x144];
	SignedValue m_textureQuality; // 0x184
	undefined m_unk0x188[0x198 - 0x188];
	Policy m_usesHardwareVertexProcessing; // 0x198
};

DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_addViewport) == 0xb0);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_createIndexBuffer) == 0x100);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_getTextureRecord) == 0x114);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_usesHardwareVertexProcessing) == 0x198);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_setDepthState) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_action0xc8) == 0xc8);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_action0x124) == 0x124);

#endif // LR2_RENDERERDISPATCH_H
