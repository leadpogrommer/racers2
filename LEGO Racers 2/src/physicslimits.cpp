#include "types.h"

// All three locations are unbacked BSS, initialized by the retail CRT callbacks.
// GLOBAL: LEGORACERS2 0x005790d8
LegoFloat g_maximumAngularSpeed;

// GLOBAL: LEGORACERS2 0x005790dc
LegoFloat g_maximumLinearSpeedSquared;

// GLOBAL: LEGORACERS2 0x00579124
LegoFloat g_maximumAngularSpeedSquared;

// STUB: LEGORACERS2 0x004a5f50
void InitializeMaximumAngularSpeed()
{
	// Original operands: float constants 0x00535370 and 0x00531894.
	g_maximumAngularSpeed = 3.1415927410125732f * 20.0f;
}

// FUNCTION: LEGORACERS2 0x004a5f80
void InitializeMaximumAngularSpeedSquared()
{
	g_maximumAngularSpeedSquared = g_maximumAngularSpeed * g_maximumAngularSpeed;
}

// STUB: LEGORACERS2 0x004a5fb0
void InitializeMaximumLinearSpeedSquared()
{
	// Original 0x0053537c stores float 331.0, not a BSS default.
	g_maximumLinearSpeedSquared = 331.0f * 331.0f;
}

// Preserve the dependencies of the three consecutive CRT initialization entries.
struct PhysicsLimitInitialization {
	PhysicsLimitInitialization()
	{
		InitializeMaximumAngularSpeed();
		InitializeMaximumAngularSpeedSquared();
		InitializeMaximumLinearSpeedSquared();
	}
};
static PhysicsLimitInitialization g_physicsLimitInitialization;
