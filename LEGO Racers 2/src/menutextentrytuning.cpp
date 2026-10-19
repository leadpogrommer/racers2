#include "menutextentrytuning.h"

// GLOBAL: LEGORACERS2 0x005619e0
LegoFloat g_menuTextEntryTuning[9];

// STUB: LEGORACERS2 0x004242b0
void InitializeMenuTextEntryTuning2Pi()
{
	g_menuTextEntryTuning[8] = 2.0f * 3.14159265f;
}

// FUNCTION: LEGORACERS2 0x004242d0
void InitializeMenuTextEntryRowSpacing()
{
	g_menuTextEntryTuning[2] = 0.06f;
	g_menuTextEntryTuning[3] = 0.06f;
}

// STUB: LEGORACERS2 0x00424300
void InitializeMenuTextEntrySpriteSpacingX()
{
	g_menuTextEntryTuning[4] = g_menuTextEntryTuning[2] * 2.0f;
}

// STUB: LEGORACERS2 0x00424330
void InitializeMenuTextEntrySpriteSpacingY()
{
	g_menuTextEntryTuning[5] = g_menuTextEntryTuning[2] * 1.25f;
}

// FUNCTION: LEGORACERS2 0x00424360
void InitializeMenuTextEntryKeySpacing()
{
	g_menuTextEntryTuning[0] = 0.25f;
	g_menuTextEntryTuning[1] = 0.0416666679f;
}

// FUNCTION: LEGORACERS2 0x00424390
void InitializeMenuTextEntrySpriteSpacing()
{
	g_menuTextEntryTuning[6] = 0.0625f;
	g_menuTextEntryTuning[7] = 0.0833333358f;
}

void InitializeMenuTextEntryTuning()
{
	InitializeMenuTextEntryTuning2Pi();
	InitializeMenuTextEntryRowSpacing();
	InitializeMenuTextEntrySpriteSpacingX();
	InitializeMenuTextEntrySpriteSpacingY();
	InitializeMenuTextEntryKeySpacing();
	InitializeMenuTextEntrySpriteSpacing();
}

struct MenuTextEntryTuningInitializer {
	MenuTextEntryTuningInitializer() { InitializeMenuTextEntryTuning(); }
};

static MenuTextEntryTuningInitializer g_menuTextEntryTuningInitializer;
