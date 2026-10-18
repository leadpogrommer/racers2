#ifndef LR2_RENDERERRESOURCECALLBACKS_H
#define LR2_RENDERERRESOURCECALLBACKS_H

#include "rendererrecords.h"
#include "types.h"

class DisplayRenderer;

RendererAuxiliaryResourceRecord* __fastcall FindAuxiliaryResourceByPath(
	DisplayRenderer* p_renderer,
	void* p_edx,
	const LegoChar* p_path
);
LegoBool32 __fastcall ClearAuxiliaryResourceCallback(
	RendererAuxiliaryResourceRecord* p_record,
	void* p_edx,
	RendererAuxiliaryResourceCallback p_callback
);
void __fastcall RegisterResourceCallback(
	DisplayRenderer* p_renderer,
	void* p_edx,
	const LegoChar* p_path,
	RendererAuxiliaryResourceCallback p_callback
);
void __fastcall UnregisterResourceCallback(
	DisplayRenderer* p_renderer,
	void* p_edx,
	RendererAuxiliaryResourceCallback p_callback
);

#endif // LR2_RENDERERRESOURCECALLBACKS_H
