#ifndef LR2_MENUMANAGERRESOURCES_H
#define LR2_MENUMANAGERRESOURCES_H

#include "menumanager.h"

void __fastcall LoadMenuManagerResources(MenuManager*, void*, LegoU32);
void __fastcall ReleaseMenuManagerResources(MenuManager*, void*, LegoU32);
void __fastcall CreateMenuManagerWidgets(MenuManager*, void*, LegoU32);
void __fastcall DestroyMenuManagerWidgets(MenuManager*, void*, LegoU32);
void __fastcall CreateCommonMenuWidgets(MenuManager*, void*);
void __fastcall DestroyCommonMenuWidgets(MenuManager*, void*);
void __fastcall CreateFrontendMenuWidgets(MenuManager*, void*);
void __fastcall DestroyFrontendMenuWidgets(MenuManager*, void*);

#endif
