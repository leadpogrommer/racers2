#ifndef LR2_SECONDARYMENUBUILDER_H
#define LR2_SECONDARYMENUBUILDER_H
#include "frontend/menu/menumanager.h"
void __fastcall BuildMenuManagerAction(MenuManager*, void*, LegoU32, LegoS32);
MenuWidgetBase* __fastcall GetInactiveMenuWidget(MenuManager*, void*, LegoU32);
void __fastcall SetMenuActionInputMask(MenuManager*, void*, LegoU32);
void __fastcall CountMenuActionOptions(MenuManager*, void*);
void __fastcall UpdateMenuActionBounds(MenuManager*, void*);
LegoFloat __fastcall MeasureMenuActionWidth(MenuManager*, void*, LegoFloat, LegoFloat);
#endif
