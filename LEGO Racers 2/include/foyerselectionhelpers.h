#ifndef LR2_FOYER_SELECTION_HELPERS_H
#define LR2_FOYER_SELECTION_HELPERS_H

#include "types.h"

struct FoyerSceneResourceView;
struct GameObject;

void __fastcall ClearFoyerNpcText(GameObject*, void*);
LegoS32 __fastcall GetFoyerObjectSelection(void* p_objectState, void*, const LegoFloat* p_statePosition);
LegoU32 __fastcall IsFoyerSceneTransitionComplete(FoyerSceneResourceView* p_resource, void*);

#endif
