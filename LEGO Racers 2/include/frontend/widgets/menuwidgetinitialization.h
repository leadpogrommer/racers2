#ifndef LR2_MENUWIDGETINITIALIZATION_H
#define LR2_MENUWIDGETINITIALIZATION_H
#include "frontend/widgets/menuintegerselector.h"
#include "frontend/widgets/menurangeslider.h"
#include "frontend/widgets/menutextgroup.h"
#include "frontend/widgets/menutoggle.h"
#include "frontend/widgets/menuvolumeslider.h"
extern LegoFloat g_menuRangeMarkerSize[2];
void __fastcall InitializeMenuVolumeControl(
	MenuVolumeSlider*,
	void*,
	LegoS32,
	const LegoFloat*,
	LegoFloat,
	LegoU32,
	LegoFloat
);
void __fastcall InitializeMenuRangeControl(
	MenuRangeSlider*,
	void*,
	LegoS32,
	const LegoFloat*,
	LegoFloat,
	LegoFloat,
	CharBuffer24*,
	LegoS32,
	LegoS32,
	LegoFloat
);
void __fastcall InitializeMenuToggleControl(MenuToggle*, void*, LegoS32, const LegoFloat*, LegoS32, LegoU32, LegoU32);
void __fastcall InitializeMenuIntegerControl(
	MenuIntegerSelector*,
	void*,
	LegoS32,
	const LegoFloat*,
	LegoS32,
	LegoS32,
	LegoS32,
	LegoU32
);
void __fastcall InitializeMenuSessionTrackGroup(MenuTextGroup*, void*, LegoS32, const LegoFloat*);
#endif
