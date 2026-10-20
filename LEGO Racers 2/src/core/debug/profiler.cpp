#include "core/debug/profiler.h"

// STUB: LEGORACERS2 0x0049adc0
void GetProfilerConfiguration(LegoFloat* p_scale, LegoU32* p_enabled, CharBuffer128* p_font)
{
	*p_enabled = 0;
	*p_scale = 66.66666412353515625f; // Original immediate: 0x42855555.
	CharBuffer128 font("game data\\fonts\\print_font.tga");
	*p_font = font;
}

// STUB: LEGORACERS2 0x004e71c0
void* BeginProfileRange(const CharBuffer24* p_name)
{
	CharBuffer128 font;
	LegoFloat scale;
	LegoU32 enabled;
	GetProfilerConfiguration(&scale, &enabled, &font);
	// Retail tests enabled immediately after the call. The configuration
	// above always writes zero, so the graph/clock path cannot execute in
	// this executable. No profile entry is allocated or name dereferenced.
	return NULL;
}

// STUB: LEGORACERS2 0x004e77f0
void EndProfileRange(void* p_range)
{
	CharBuffer128 font;
	LegoFloat scale;
	LegoU32 enabled;
	GetProfilerConfiguration(&scale, &enabled, &font);
	// The same unconditional disabled configuration precedes every access
	// to p_range in retail, including the NULL returned by BeginProfileRange.
}
