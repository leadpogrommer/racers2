#include "input/feedback/joystickperiodic.h"

#include "decomp.h"
#include "input/feedback/joystickparams.h"

#include <stddef.h>

namespace
{

struct SineDescriptionStorage {
	DIEFFECT effect;
	DIPERIODIC periodic;
	LONG direction[3];
};

struct EnvelopedSineDescriptionStorage {
	DIEFFECT effect;
	DIPERIODIC periodic;
	DIENVELOPE envelope;
	LONG direction[3];
};

DECOMP_SIZE_ASSERT(SineDescriptionStorage, 0x54);
DECOMP_STATIC_ASSERT(offsetof(SineDescriptionStorage, periodic) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(SineDescriptionStorage, direction) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(EnvelopedSineDescriptionStorage, periodic) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(EnvelopedSineDescriptionStorage, envelope) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(EnvelopedSineDescriptionStorage, direction) == 0x5c);

// GLOBAL: LEGORACERS2 0x006e4774
static SineDescriptionStorage g_sineDescription;
// GLOBAL: LEGORACERS2 0x0054b128
static LegoU32 g_sineDescriptionNeedsInitialization = TRUE;

// GLOBAL: LEGORACERS2 0x006e4570
static EnvelopedSineDescriptionStorage g_envelopedSineDescription;
// GLOBAL: LEGORACERS2 0x0054b130
static LegoU32 g_envelopedSineDescriptionNeedsInitialization = TRUE;

static void InitializeEffect(
	DIEFFECT& p_effect,
	LegoU32 p_axes,
	LONG* p_direction,
	LPDIENVELOPE p_envelope,
	LPVOID p_typeSpecific
)
{
	p_effect.dwSize = sizeof(DIEFFECT);
	p_effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	p_effect.dwDuration = INFINITE;
	p_effect.dwSamplePeriod = 0;
	p_effect.dwGain = DI_FFNOMINALMAX;
	p_effect.dwTriggerButton = DIEB_NOTRIGGER;
	p_effect.dwTriggerRepeatInterval = 0;
	p_effect.cAxes = p_axes;
	p_effect.rgdwAxes = g_forceFeedbackAxisOffsets;
	p_effect.rglDirection = p_direction;
	p_effect.lpEnvelope = p_envelope;
	p_effect.cbTypeSpecificParams = sizeof(DIPERIODIC);
	p_effect.lpvTypeSpecificParams = p_typeSpecific;
	p_effect.dwStartDelay = 0;
}

static LegoFloat ClampSineFrequency(LegoFloat p_frequency)
{
	LegoFloat frequency = 300.0f;
	if (p_frequency <= 300.0f) {
		frequency = p_frequency;
		if (p_frequency < 1.0f) {
			frequency = 1.0f;
		}
	}
	return frequency;
}

static LegoS32 RoundUsingCurrentFpuMode(LegoFloat p_value)
{
	LegoS32 result;
	__asm {
		fld p_value
		fistp result
	}
	return result;
}

} // namespace

// STUB: LEGORACERS2 0x004ed030
DIEFFECT* BuildSineDescription(LegoU32 p_axes, LegoFloat p_frequency, LegoFloat p_magnitude)
{
	if (g_sineDescriptionNeedsInitialization != FALSE) {
		g_sineDescriptionNeedsInitialization = FALSE;
		InitializeEffect(
			g_sineDescription.effect,
			p_axes,
			g_sineDescription.direction,
			NULL,
			&g_sineDescription.periodic
		);

		g_sineDescription.periodic.dwMagnitude = 0;
		g_sineDescription.periodic.lOffset = 0;
		g_sineDescription.periodic.dwPhase = 0;
		g_sineDescription.periodic.dwPeriod = 0;

		g_sineDescription.direction[0] = 1;
		g_sineDescription.direction[1] = 1;
		g_sineDescription.direction[2] = 1;
	}

	LegoFloat frequency = ClampSineFrequency(p_frequency);
	LegoFloat period = ((1.0f / frequency) * 1000.0f) * 10000.0f;
	g_sineDescription.periodic.dwPeriod = RoundUsingCurrentFpuMode(period);

	LegoFloat magnitude = p_magnitude * 10000.0f;
	g_sineDescription.periodic.dwMagnitude = RoundUsingCurrentFpuMode(magnitude);

	return &g_sineDescription.effect;
}

// STUB: LEGORACERS2 0x004ed250
DIEFFECT* BuildEnvelopedSineDescription(
	LegoU32 p_axes,
	LegoFloat p_frequency,
	LegoFloat p_magnitude,
	LegoFloat p_duration
)
{
	if (g_envelopedSineDescriptionNeedsInitialization != FALSE) {
		g_envelopedSineDescriptionNeedsInitialization = FALSE;
		InitializeEffect(
			g_envelopedSineDescription.effect,
			p_axes,
			g_envelopedSineDescription.direction,
			&g_envelopedSineDescription.envelope,
			&g_envelopedSineDescription.periodic
		);

		g_envelopedSineDescription.periodic.dwMagnitude = 0;
		g_envelopedSineDescription.periodic.lOffset = 0;
		g_envelopedSineDescription.periodic.dwPhase = 0;
		g_envelopedSineDescription.periodic.dwPeriod = 0;

		g_envelopedSineDescription.envelope.dwSize = sizeof(DIENVELOPE);
		g_envelopedSineDescription.envelope.dwAttackLevel = 100;
		g_envelopedSineDescription.envelope.dwAttackTime = 100000;
		g_envelopedSineDescription.envelope.dwFadeLevel = 100;
		g_envelopedSineDescription.envelope.dwFadeTime = 300000;

		g_envelopedSineDescription.direction[0] = 1;
		g_envelopedSineDescription.direction[1] = 1;
		g_envelopedSineDescription.direction[2] = 1;
	}

	LegoFloat frequency = ClampSineFrequency(p_frequency);
	LegoFloat period = ((1.0f / frequency) * 1000.0f) * 10000.0f;
	g_envelopedSineDescription.periodic.dwPeriod = RoundUsingCurrentFpuMode(period);

	LegoFloat magnitude = p_magnitude * 10000.0f;
	g_envelopedSineDescription.periodic.dwMagnitude = RoundUsingCurrentFpuMode(magnitude);

	LegoFloat duration = p_duration * 1000000.0f;
	g_envelopedSineDescription.effect.dwDuration = RoundUsingCurrentFpuMode(duration);

	LegoFloat attackTime = duration * 0.2f;
	g_envelopedSineDescription.envelope.dwAttackTime = RoundUsingCurrentFpuMode(attackTime);

	LegoFloat fadeTime = duration * 0.33333334f;
	g_envelopedSineDescription.envelope.dwFadeTime = RoundUsingCurrentFpuMode(fadeTime);

	return &g_envelopedSineDescription.effect;
}
