#ifndef LR2_RENDERERDISPATCH_H
#define LR2_RENDERERDISPATCH_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

class DisplayRenderer;

// View of an observed game dispatch slot. This does not construct a table or
// propose a source class hierarchy. The preceding slots retain their offsets.
struct RendererDispatchView {
	typedef LegoBool32(__fastcall* Policy)(DisplayRenderer*, void*);
	typedef void(__fastcall* SetValue)(DisplayRenderer*, void*, LegoU32);
	undefined m_unk0x0[0x98];
	SetValue m_setDepthState; // 0x98
	undefined m_unk0x9c[0xc8 - 0x9c];
	SetValue m_action0xc8; // 0xc8
	undefined m_unk0xcc[0x124 - 0xcc];
	SetValue m_action0x124; // 0x124
	undefined m_unk0x128[0x198 - 0x128];
	Policy m_usesHardwareVertexProcessing; // 0x198
};

DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_usesHardwareVertexProcessing) == 0x198);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_setDepthState) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_action0xc8) == 0xc8);
DECOMP_STATIC_ASSERT(offsetof(RendererDispatchView, m_action0x124) == 0x124);

#endif // LR2_RENDERERDISPATCH_H
