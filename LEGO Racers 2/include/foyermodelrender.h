#ifndef LR2_FOYERMODELRENDER_H
#define LR2_FOYERMODELRENDER_H

#include "gamemanager.h"

struct PhysicalLifecycleState;

struct FoyerModelRenderDispatch {
	GameObjectDispatchView m_object;
	const void*(__fastcall* m_getTypeAddress)(GameObject*, void*);
};
DECOMP_SIZE_ASSERT(FoyerModelRenderDispatch, 0x54);
extern const FoyerModelRenderDispatch g_foyerModelRenderDispatch;

GameObject* __fastcall ConstructFoyerModelRenderObject(GameObject*, void*, LegoU32, GameObjectTemplate*);
void __fastcall DestructFoyerModelRenderObject(GameObject*, void*);
GameObject* __fastcall DestroyFoyerModelRenderObject(GameObject*, void*, LegoU32);
void InitializeFoyerModelRenderBase(GameObject*, GameObjectTemplate*);
void ReleaseFoyerModelRenderBase(GameObject*);
LegoU32 __fastcall IntegrateFoyerLifecycleMotion(PhysicalLifecycleState*, void*);
LegoU32 __fastcall ApplyFoyerLifecycleForces(PhysicalLifecycleState*, void*);
LegoU32 __fastcall IntegrateFoyerModelRenderMotion(GameObject*, void*);
LegoU32 __fastcall ApplyFoyerModelRenderForces(GameObject*, void*);
void __fastcall DrawFoyerModelRenderObject(GameObject*, void*);
void __fastcall InitializeFoyerModelRenderObject(GameObject*, void* = NULL);
void RemoveFoyerModelRenderObject(GameObject*);
void ApplyFoyerModelRenderTransform(GameObject*, const LegoFloat*, const LegoFloat*);
LegoBool32 __fastcall UpdateFoyerModelRenderRegion(GameObject*, void*, void*);

#endif
