#include "rendereranimationresource.h"

#include "applicationpolicy.h"
#include "renderer.h"

#include <string.h>

extern LegoU32 ApplicationTruePolicy();
extern "C" int __cdecl _purecall();

typedef LegoBool32(__fastcall* RecordPolicy)(RendererAnimationResourceRecord*, void*);
typedef void(__fastcall* RecordAction)(RendererAnimationResourceRecord*, void*);
typedef void(__fastcall* RecordDiagnostic)(RendererAnimationResourceRecord*, void*, const LegoChar*);

LegoBool32 __fastcall RendererAnimationReturnsTrue(RendererAnimationResourceRecord* p_record, void*)
{
	return p_record->ReturnsTrue();
}

void __fastcall RendererAnimationNoOp(RendererAnimationResourceRecord* p_record, void*)
{
	p_record->NoOp();
}

void __fastcall RendererAnimationRelease(RendererAnimationResourceRecord* p_record, void*)
{
	p_record->ReleaseResourceReference();
}

LegoBool32 __fastcall RendererAnimationTruePolicy(RendererAnimationResourceRecord*, void*)
{
	return (LegoBool32) ApplicationTruePolicy();
}

void __fastcall RendererAnimationDiagnostic(RendererAnimationResourceRecord* p_record, void*, const LegoChar* p_message)
{
	p_record->DiagnosticCallback(p_message);
}

LegoBool32 __fastcall RendererAnimationDevelopmentMode(RendererAnimationResourceRecord* p_record, void*)
{
	return p_record->IsDevelopmentMode();
}

extern const RendererAnimationResourceAbstractDispatch g_rendererAnimationResourceAbstractDispatch = {
	{_purecall, _purecall, _purecall, _purecall, _purecall},
};

extern const RendererAnimationResourceDispatch g_rendererAnimationResourceDispatch = {
	_purecall,
	RendererAnimationReturnsTrue,
	RendererAnimationNoOp,
	RendererAnimationRelease,
	RendererAnimationTruePolicy,
	RendererAnimationDiagnostic,
	RendererAnimationDevelopmentMode,
};

void RendererAnimationResourceRecord::Initialize()
{
	m_vtable = (void*) &g_rendererAnimationResourceAbstractDispatch;
	m_flags = (LegoU16) ((m_flags & 0xc008) | 0x0008);
	m_vtable = (void*) &g_rendererAnimationResourceDispatch;
}

LegoBool32 RendererAnimationResourceRecord::ReturnsTrue()
{
	return TRUE;
}

// The compiler folds this identical path/resource prefix into the cache setter.
// FUNCTION: LEGORACERS2 0x004c0070 FOLDED
LegoBool32 RendererAnimationResourceRecord::SetPath(const LegoChar* p_path)
{
	strcpy(m_path, p_path);
	m_resourceId = (LegoS16) g_displayRenderer->CreateTextureResource("", 0, 0, 0, 0);
	return TRUE;
}

void RendererAnimationResourceRecord::NoOp()
{
}

void RendererAnimationResourceRecord::DiagnosticCallback(const LegoChar* p_message)
{
	OnApplicationDiagnostic(p_message);
}

LegoBool32 RendererAnimationResourceRecord::IsDevelopmentMode()
{
	return IsApplicationDevelopmentMode();
}

// FUNCTION: LEGORACERS2 0x004ea440
void RendererAnimationResourceRecord::ReleaseResourceReference()
{
	g_displayRenderer->ReleaseTextureResourceReference(m_resourceId, 0);
}

// STUB: LEGORACERS2 0x004ea460
RendererAnimationResourceRecord* DisplayRenderer::FindOrAllocateAnimationResource(
	const LegoChar* p_path,
	LegoBool32 p_loadNow
)
{
	LegoS32 i;
	for (i = 0; i < 4; ++i) {
		RendererAnimationResourceRecord* record = &m_animationResources[i];
		if ((record->m_flags & 1) != 0 && strcmp(record->m_path, p_path) == 0) {
			return record;
		}
	}

	for (i = 0; i < 4; ++i) {
		RendererAnimationResourceRecord* record = &m_animationResources[i];
		if ((record->m_flags & 1) == 0) {
			record->m_flags |= 3;
			record->SetPath(p_path);
			if (p_loadNow != FALSE) {
				((RecordPolicy*) record->m_vtable)[1](record, NULL);
			}
			return record;
		}
	}
	return NULL;
}
