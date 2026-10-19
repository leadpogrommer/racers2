#ifndef LR2_GAMEOBJECTRENDERPREP_H
#define LR2_GAMEOBJECTRENDERPREP_H

#include "types.h"

class Vector3;

// Receiver-only projections of rendering helpers observed on game objects.
void __cdecl SetRenderProjectionRatio(LegoFloat p_scale, LegoFloat p_limit);
void __fastcall PrepareGameObjectForRendering(void* p_object, void*);
LegoFloat __fastcall GetGameObjectViewDepth(void* p_camera, void*, const Vector3* p_point);
void __fastcall UpdateGameObjectRenderGeometry(void* p_object, void*);
LegoBool32 __fastcall SetGameObjectLookAt(
	void* p_object,
	void*,
	const Vector3* p_position,
	const Vector3* p_target,
	const Vector3* p_up
);
void __fastcall SetGameObjectViewport(
	void* p_object,
	void*,
	LegoS32 p_left,
	LegoS32 p_top,
	LegoS32 p_width,
	LegoS32 p_height
);

extern LegoFloat g_renderProjectionRatio;
extern void* g_unk0x00679ef0;
extern void* g_unk0x0067bafc;

#endif // LR2_GAMEOBJECTRENDERPREP_H
