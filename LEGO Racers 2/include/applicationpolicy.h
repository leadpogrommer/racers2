#ifndef LR2_APPLICATIONPOLICY_H
#define LR2_APPLICATIONPOLICY_H

#include "types.h"

#include <windows.h>

// Recovered leaf policies bound into the complete application dispatch tables.
// FUNCTION: LEGORACERS2 0x00403910 FOLDED
// IsApplicationDevelopmentMode
LegoBool32 IsApplicationDevelopmentMode();
LegoU32 GetApplicationHeapReserve();
LegoU32 GetApplicationIconResourceId();
void WINAPI OnApplicationDiagnostic(const LegoChar* p_message);

#endif // LR2_APPLICATIONPOLICY_H
