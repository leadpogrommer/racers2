#include "menulayouttuning.h"

// GLOBAL: LEGORACERS2 0x00561758
LegoFloat g_menuLayoutMarginX;
// GLOBAL: LEGORACERS2 0x0056175c
LegoFloat g_menuLayoutMarginY;

// FUNCTION: LEGORACERS2 0x0041c1e0
void InitializeMenuLayoutMargins()
{
	g_menuLayoutMarginX = 0.04f;
	g_menuLayoutMarginY = 0.02f;
}

struct MenuLayoutMarginsInitializer {
	MenuLayoutMarginsInitializer() { InitializeMenuLayoutMargins(); }
};

static MenuLayoutMarginsInitializer g_menuLayoutMarginsInitializer;
