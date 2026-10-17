#ifndef LR2_APPLICATIONPOLICY_H
#define LR2_APPLICATIONPOLICY_H

#include "types.h"

#include <windows.h>

// Recovered leaf policies bound into the complete application dispatch tables.
LegoBool32 IsApplicationDevelopmentMode();
LegoU32 GetApplicationHeapReserve();
LegoU32 GetApplicationIconResourceId();
void WINAPI OnApplicationDiagnostic(const LegoChar* p_message);

#endif // LR2_APPLICATIONPOLICY_H
