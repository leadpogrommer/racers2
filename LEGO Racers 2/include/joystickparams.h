#ifndef LR2_JOYSTICKPARAMS_H
#define LR2_JOYSTICKPARAMS_H

#include "directx8.h"
#include "types.h"

extern DWORD g_forceFeedbackAxisOffsets[2];

// Retail 0x00473140: genuine NULL-return body, folded with the existing zero
// leaf by the linker. Its extent is inventory-only, not an extra match row.

DIEFFECT* BuildSquareDescription(LegoU32 p_axes, LegoFloat p_first, LegoFloat p_second, LegoFloat p_third);
DIEFFECT* BuildConstantForceDescription(LegoU32 p_axes, LegoFloat p_x, LegoFloat p_y);
DIEFFECT* BuildSpringDescription(LegoU32 p_axes, LegoFloat p_coefficient);
DIEFFECT* BuildFrictionDescription(LegoU32 p_axes, LegoFloat p_coefficient);
DIEFFECT* BuildEnvelopedConstantDescription(LegoU32 p_axes, LegoFloat p_x, LegoFloat p_y);

#endif // LR2_JOYSTICKPARAMS_H
