#ifndef LR2_GAMEOBJECTRENDERPREP_H
#define LR2_GAMEOBJECTRENDERPREP_H

#include "types.h"

// Receiver-only projections of rendering helpers observed on game objects.
void __cdecl SetRenderProjectionRatio(LegoFloat p_scale, LegoFloat p_limit);
void __fastcall PrepareGameObjectForRendering(void* p_object, void*);
void __fastcall UpdateGameObjectRenderGeometry(void* p_object, void*);

extern LegoFloat g_renderProjectionRatio;
extern void* g_unk0x00679ef0;
extern void* g_unk0x0067bafc;

#endif // LR2_GAMEOBJECTRENDERPREP_H
