#ifndef LR2_CONTROLLERFOYERTASKMANAGER_H
#define LR2_CONTROLLERFOYERTASKMANAGER_H

#include "types.h"

void* __cdecl GetControllerTaskManager();
void __fastcall SetControllerTaskActionStatus(void* p_manager, void*, LegoU32 p_index, LegoU32 p_status);
void __fastcall SetControllerTaskActionRecord(void* p_manager, void*, LegoU32 p_index, const LegoU32* p_words);

void __cdecl ResetControllerTaskOverlayState();
#endif // LR2_CONTROLLERFOYERTASKMANAGER_H
