#ifndef LR2_FOYER_SCENE_CONTROLLER_ACTIONS_H
#define LR2_FOYER_SCENE_CONTROLLER_ACTIONS_H

#include "game/objects/gamemanager.h"

void __fastcall SetFoyerControllerMenuLoop(GameObject*, void*);
void __fastcall SetFoyerControllerRunMenu(GameObject*);
void __fastcall StartFoyerControllerRunLoop(GameObject*);
void __fastcall SetFoyerControllerActive(GameObject*, void*, LegoU32);
void __fastcall RestoreFoyerControllerActionState(GameObject*, void*);
void __fastcall SetFoyerMenuSceneActive(GameObject*, void*, GameObject*, LegoU32);

#endif // LR2_FOYER_SCENE_CONTROLLER_ACTIONS_H
