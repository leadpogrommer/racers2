#include "inputformats.h"

#include "decomp.h"

#include <stddef.h>

// Retail's SDK mouse format uses NULL GUID filters for buttons. Wine's current
// c_dfDIMouse uses GUID_Button instead. Retain the observed less restrictive
// filters, while keeping the real SDK structures, constants and axis GUIDs.
static const DIOBJECTDATAFORMAT g_mouseObjectFormats[7] = {
	{&GUID_XAxis, offsetof(DIMOUSESTATE, lX), DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_YAxis, offsetof(DIMOUSESTATE, lY), DIDFT_AXIS | DIDFT_ANYINSTANCE, 0},
	{&GUID_ZAxis, offsetof(DIMOUSESTATE, lZ), DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{NULL, offsetof(DIMOUSESTATE, rgbButtons) + 0, DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
	{NULL, offsetof(DIMOUSESTATE, rgbButtons) + 1, DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0},
	{NULL, offsetof(DIMOUSESTATE, rgbButtons) + 2, DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
	{NULL, offsetof(DIMOUSESTATE, rgbButtons) + 3, DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0},
};

const DIDATAFORMAT g_mouseInputDataFormat = {
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_RELAXIS,
	sizeof(DIMOUSESTATE),
	7,
	const_cast<LPDIOBJECTDATAFORMAT>(g_mouseObjectFormats),
};

DECOMP_SIZE_ASSERT(DIOBJECTDATAFORMAT, 0x10);
DECOMP_SIZE_ASSERT(DIDATAFORMAT, 0x18);
