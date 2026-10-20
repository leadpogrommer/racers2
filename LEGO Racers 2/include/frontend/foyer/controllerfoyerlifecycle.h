#ifndef LR2_CONTROLLERFOYERLIFECYCLE_H
#define LR2_CONTROLLERFOYERLIFECYCLE_H

#include "game/controller/gamecontroller.h"

class FoyerManager;
struct CharBuffer128;
void __fastcall ResetFoyerGameControllerState(GameController*, void*);
void __fastcall SetControllerFaceTexturesLoaded(GameController*, void*, LegoU32);
void __fastcall ComposeFoyerSceneResourcePath(FoyerManager*, void*, CharBuffer128*, LegoS32, LegoS32, LegoS32);

void __fastcall PreloadOtherFoyerSceneResources(FoyerManager*, void*, LegoS32);
#endif // LR2_CONTROLLERFOYERLIFECYCLE_H
