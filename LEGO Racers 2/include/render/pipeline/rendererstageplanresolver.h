#ifndef LR2_RENDERERSTAGEPLANRESOLVER_H
#define LR2_RENDERERSTAGEPLANRESOLVER_H

#include "render/pipeline/rendererstageplan.h"
#include "types.h"

class DisplayRenderer;

// Resolve a request's active texture stages against renderer capabilities and
// the cached default/profile plans. The explicit EDX parameter preserves the
// original thiscall stack ABI when exposed as a free function.
void __fastcall ResolveRendererTextureStagePlan(
	DisplayRenderer* p_renderer,
	void*,
	RendererStagePlanRequest* p_request
);

#endif // LR2_RENDERERSTAGEPLANRESOLVER_H
