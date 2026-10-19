#ifndef LR2_MODELRENDERING_H
#define LR2_MODELRENDERING_H

#include "types.h"

struct ModelResource;
struct GameObjectModelBinding;
class DisplayRenderer;
class Vector3;

void __fastcall DrawModelResource(ModelResource*, void*, GameObjectModelBinding*);
void __fastcall UpdateModelResourceVisibility(ModelResource*, void*, GameObjectModelBinding*);
LegoS32 __fastcall GetModelPointSortKey(DisplayRenderer*, void*, const Vector3*);
LegoS32 __fastcall GetModelDepthSortKey(DisplayRenderer*, void*, LegoFloat);

#endif
