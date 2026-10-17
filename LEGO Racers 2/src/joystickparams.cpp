#include "joystickparams.h"

#include "decomp.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

DECOMP_SIZE_ASSERT(DIEFFECT, 0x38);
DECOMP_SIZE_ASSERT(DICONDITION, 0x18);
DECOMP_SIZE_ASSERT(DIENVELOPE, 0x14);

// GLOBAL: LEGORACERS2 0x0054b0fc
DWORD g_forceFeedbackAxisOffsets[2] = {0, 4};

struct ConstantForceDescription {
	DIEFFECT effect;
	DICONSTANTFORCE force;
	LONG direction[2];
};
struct ConditionDescription {
	DIEFFECT effect;
	DICONDITION condition;
	LONG direction[2];
};
struct EnvelopedConstantDescription {
	DIEFFECT effect;
	DICONSTANTFORCE force;
	DIENVELOPE envelope;
	LONG direction[2];
};
DECOMP_STATIC_ASSERT(offsetof(ConstantForceDescription, force) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(ConstantForceDescription, direction) == 0x3c);
DECOMP_STATIC_ASSERT(offsetof(ConditionDescription, condition) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(ConditionDescription, direction) == 0x50);
DECOMP_STATIC_ASSERT(offsetof(EnvelopedConstantDescription, force) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(EnvelopedConstantDescription, envelope) == 0x3c);
DECOMP_STATIC_ASSERT(offsetof(EnvelopedConstantDescription, direction) == 0x50);

// Persistent retail workspaces. The descriptor is cleared only on first use;
// callers and the DirectInput API receive pointers to this stable storage.
// GLOBAL: LEGORACERS2 0x006e45e0
static ConstantForceDescription g_constantDescription;
// GLOBAL: LEGORACERS2 0x006e46b8
static ConditionDescription g_springDescription;
// GLOBAL: LEGORACERS2 0x006e463c
static ConditionDescription g_frictionDescription;
// GLOBAL: LEGORACERS2 0x006e4714
static EnvelopedConstantDescription g_envelopedConstantDescription;

// GLOBAL: LEGORACERS2 0x0054b108
static LegoU32 g_constantDescriptionNeedsInit = TRUE;
// GLOBAL: LEGORACERS2 0x0054b110
static LegoU32 g_springDescriptionNeedsInit = TRUE;
// GLOBAL: LEGORACERS2 0x0054b118
static LegoU32 g_frictionDescriptionNeedsInit = TRUE;
// GLOBAL: LEGORACERS2 0x0054b120
static LegoU32 g_envelopedConstantDescriptionNeedsInit = TRUE;

static void InitializeEffectDescription(DIEFFECT& p_effect, DWORD p_axes, DWORD p_duration)
{
	memset(&p_effect, 0, sizeof(p_effect));
	p_effect.dwSize = sizeof(p_effect);
	p_effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	p_effect.dwDuration = p_duration;
	p_effect.dwGain = DI_FFNOMINALMAX;
	p_effect.dwTriggerButton = DIEB_NOTRIGGER;
	p_effect.cAxes = p_axes;
	p_effect.rgdwAxes = g_forceFeedbackAxisOffsets;
}

// The argument enforces the original float store/reload before FISTP. A C++
// integer cast would truncate regardless of the active x87 rounding mode.
static LONG RoundEffectFloat(LegoFloat p_value)
{
	LONG result;
	__asm {
		fld p_value
		fistp result
	}
	return result;
}

static void ClampForcePair(LegoFloat& p_x, LegoFloat& p_y)
{
	if (!(p_x >= -0.99f)) {
		p_x = -0.99f;
	}
	else if (p_y > 0.99f) {
		// Retail tests Y here, even though it assigns X.
		p_x = 0.99f;
	}
	if (!(p_y >= -0.99f)) {
		p_y = -0.99f;
	}
	else if (p_y > 0.99f) {
		p_y = 0.99f;
	}
}

static void UpdateConstantForce(
	DIEFFECT& p_effect,
	DICONSTANTFORCE& p_force,
	LegoU32 p_axes,
	LegoFloat p_x,
	LegoFloat p_y
)
{
	p_effect.cAxes = p_axes;
	if (p_axes == 1) {
		p_effect.rglDirection[0] = 0;
		p_force.lMagnitude = RoundEffectFloat(p_x * 10000.0f);
	}
	else {
		p_force.lMagnitude = RoundEffectFloat((LegoFloat) (sqrt(p_x * p_x + p_y * p_y) * 10000.0f));
		p_effect.rglDirection[0] = RoundEffectFloat(p_x * 10000.0f);
		p_effect.rglDirection[1] = RoundEffectFloat(p_y * 10000.0f);
	}
}

// The linker folds this genuine NULL-return body with the existing zero leaf.
// Its original extent stays inventory-only; see joystickparams.h.
// The source helper retains the original caller-cleaned argument layout.
DIEFFECT* BuildSquareDescription(LegoU32, LegoFloat, LegoFloat, LegoFloat)
{
	// This linked retail helper really returns NULL; keep its unsupported path.
	return NULL;
}

// STUB: LEGORACERS2 0x004ec820
DIEFFECT* BuildConstantForceDescription(LegoU32 p_axes, LegoFloat p_x, LegoFloat p_y)
{
	ClampForcePair(p_x, p_y);
	if (g_constantDescriptionNeedsInit) {
		g_constantDescriptionNeedsInit = FALSE;
		InitializeEffectDescription(g_constantDescription.effect, 0, INFINITE);
		g_constantDescription.effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		g_constantDescription.effect.lpvTypeSpecificParams = &g_constantDescription.force;
		g_constantDescription.effect.rglDirection = g_constantDescription.direction;
	}
	UpdateConstantForce(g_constantDescription.effect, g_constantDescription.force, p_axes, p_x, p_y);
	return &g_constantDescription.effect;
}

// STUB: LEGORACERS2 0x004eca50
DIEFFECT* BuildSpringDescription(LegoU32 p_axes, LegoFloat p_coefficient)
{
	if (g_springDescriptionNeedsInit) {
		g_springDescriptionNeedsInit = FALSE;
		InitializeEffectDescription(g_springDescription.effect, p_axes, INFINITE);
		g_springDescription.effect.cbTypeSpecificParams = sizeof(DICONDITION);
		g_springDescription.effect.lpvTypeSpecificParams = &g_springDescription.condition;
		g_springDescription.effect.rglDirection = g_springDescription.direction;
		memset(&g_springDescription.condition, 0, sizeof(g_springDescription.condition));
		g_springDescription.condition.dwPositiveSaturation = DI_FFNOMINALMAX;
		g_springDescription.condition.dwNegativeSaturation = DI_FFNOMINALMAX;
	}
	g_springDescription.condition.lPositiveCoefficient = RoundEffectFloat(p_coefficient * 10000.0f);
	g_springDescription.condition.lNegativeCoefficient = RoundEffectFloat(p_coefficient * 10000.0f);
	return &g_springDescription.effect;
}

// STUB: LEGORACERS2 0x004ecbe0
DIEFFECT* BuildFrictionDescription(LegoU32 p_axes, LegoFloat p_coefficient)
{
	if (g_frictionDescriptionNeedsInit) {
		g_frictionDescriptionNeedsInit = FALSE;
		InitializeEffectDescription(g_frictionDescription.effect, p_axes, INFINITE);
		g_frictionDescription.effect.cbTypeSpecificParams = sizeof(DICONDITION);
		g_frictionDescription.effect.lpvTypeSpecificParams = &g_frictionDescription.condition;
		g_frictionDescription.effect.rglDirection = g_frictionDescription.direction;
		memset(&g_frictionDescription.condition, 0, sizeof(g_frictionDescription.condition));
		g_frictionDescription.condition.dwPositiveSaturation = DI_FFNOMINALMAX;
		g_frictionDescription.condition.dwNegativeSaturation = DI_FFNOMINALMAX;
	}
	g_frictionDescription.condition.lPositiveCoefficient = RoundEffectFloat(p_coefficient * 10000.0f);
	g_frictionDescription.condition.lNegativeCoefficient = RoundEffectFloat(p_coefficient * 10000.0f);
	return &g_frictionDescription.effect;
}

// STUB: LEGORACERS2 0x004ecd70
DIEFFECT* BuildEnvelopedConstantDescription(LegoU32 p_axes, LegoFloat p_x, LegoFloat p_y)
{
	ClampForcePair(p_x, p_y);
	if (g_envelopedConstantDescriptionNeedsInit) {
		g_envelopedConstantDescriptionNeedsInit = FALSE;
		InitializeEffectDescription(g_envelopedConstantDescription.effect, 0, 100000);
		g_envelopedConstantDescription.effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		g_envelopedConstantDescription.effect.lpvTypeSpecificParams = &g_envelopedConstantDescription.force;
		g_envelopedConstantDescription.effect.rglDirection = g_envelopedConstantDescription.direction;
		memset(&g_envelopedConstantDescription.envelope, 0, sizeof(g_envelopedConstantDescription.envelope));
		g_envelopedConstantDescription.envelope.dwSize = sizeof(DIENVELOPE);
		g_envelopedConstantDescription.envelope.dwAttackLevel = 10000;
		g_envelopedConstantDescription.envelope.dwAttackTime = 100000;
		g_envelopedConstantDescription.envelope.dwFadeTime = 300000;
		g_envelopedConstantDescription.envelope.dwFadeLevel = 100;
		// Retail prepares the envelope but leaves effect.lpEnvelope NULL.
	}
	UpdateConstantForce(g_envelopedConstantDescription.effect, g_envelopedConstantDescription.force, p_axes, p_x, p_y);
	return &g_envelopedConstantDescription.effect;
}
