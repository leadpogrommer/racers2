#ifndef LR2_JOYSTICKPERIODIC_H
#define LR2_JOYSTICKPERIODIC_H

#include "directx8.h"
#include "types.h"

DIEFFECT* BuildSineDescription(LegoU32 p_axes, LegoFloat p_frequency, LegoFloat p_magnitude);
DIEFFECT* BuildEnvelopedSineDescription(
	LegoU32 p_axes,
	LegoFloat p_frequency,
	LegoFloat p_magnitude,
	LegoFloat p_duration
);

#endif // LR2_JOYSTICKPERIODIC_H
