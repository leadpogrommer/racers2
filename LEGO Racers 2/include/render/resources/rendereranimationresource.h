#ifndef LR2_RENDERERANIMATIONRESOURCE_H
#define LR2_RENDERERANIMATIONRESOURCE_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct RendererAnimationResourceRecord {
	typedef LegoBool32(__fastcall* Policy)(RendererAnimationResourceRecord*, void*);
	typedef void(__fastcall* Action)(RendererAnimationResourceRecord*, void*);
	typedef void(__fastcall* Diagnostic)(RendererAnimationResourceRecord*, void*, const LegoChar*);

	void* m_vtable;
	LegoS16 m_resourceId;
	LegoU16 m_flags;
	LegoChar m_path[128];
	undefined m_unk0x88[0x120 - 0x88];
	void* m_unk0x120;

	void Initialize();
	LegoBool32 SetPath(const LegoChar* p_path);
	LegoBool32 ReturnsTrue();
	void NoOp();
	void ReleaseResourceReference();
	void DiagnosticCallback(const LegoChar* p_message);
	LegoBool32 IsDevelopmentMode();
};

DECOMP_SIZE_ASSERT(RendererAnimationResourceRecord, 0x124);
DECOMP_STATIC_ASSERT(offsetof(RendererAnimationResourceRecord, m_resourceId) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(RendererAnimationResourceRecord, m_flags) == 0x06);
DECOMP_STATIC_ASSERT(offsetof(RendererAnimationResourceRecord, m_path) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererAnimationResourceRecord, m_unk0x88) == 0x88);
DECOMP_STATIC_ASSERT(offsetof(RendererAnimationResourceRecord, m_unk0x120) == 0x120);

struct RendererAnimationResourceDispatch {
	typedef int(__cdecl* PureCall)();
	typedef LegoBool32(__fastcall* Policy)(RendererAnimationResourceRecord*, void*);
	typedef void(__fastcall* Action)(RendererAnimationResourceRecord*, void*);
	typedef void(__fastcall* Diagnostic)(RendererAnimationResourceRecord*, void*, const LegoChar*);

	PureCall m_abstract;
	Policy m_returnsTrue;
	Action m_noOp;
	Action m_releaseResourceReference;
	Policy m_truePolicy;
	Diagnostic m_diagnostic;
	Policy m_developmentMode;
};

struct RendererAnimationResourceAbstractDispatch {
	typedef int(__cdecl* PureCall)();
	PureCall m_callbacks[5];
};

DECOMP_SIZE_ASSERT(RendererAnimationResourceDispatch, 0x1c);
DECOMP_SIZE_ASSERT(RendererAnimationResourceAbstractDispatch, 0x14);
extern const RendererAnimationResourceDispatch g_rendererAnimationResourceDispatch;
extern const RendererAnimationResourceAbstractDispatch g_rendererAnimationResourceAbstractDispatch;

#endif // LR2_RENDERERANIMATIONRESOURCE_H
