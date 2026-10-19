#ifndef LR2_MENUMANAGERCURRENTACTION_H
#define LR2_MENUMANAGERCURRENTACTION_H

#include "menucharacterpreviewselector.h"
#include "menuchoiceselector.h"
#include "menumanager.h"

struct SecondaryManager;
class DisplayRenderer;

void __fastcall UpdateMenuManagerCurrentAction(MenuManager* p_manager, void* p_unusedEdx);
void __fastcall InitializeMenuChoiceSelectorFromMode(
	MenuChoiceSelector* p_selector,
	void* p_unusedEdx,
	LegoS32 p_initialValue,
	const LegoFloat* p_position,
	LegoS32 p_selection,
	LegoS32 p_firstString,
	LegoS32 p_secondString,
	LegoS32 p_leftString,
	LegoS32 p_rightString,
	LegoFloat p_maxScale,
	LegoU32 p_createArrows,
	LegoU32 p_state
);
LegoFloat __fastcall GetMenuManagerFloatValue(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id);
LegoFloat __fastcall GetMenuManagerFloatOption(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id);
LegoU32 __fastcall GetMenuManagerWordOption(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id);
LegoU32 __fastcall GetMenuManagerInitialValue(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id);
LegoU32 __fastcall GetMenuManagerWordAtEight(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id);
LegoU32 __fastcall GetMenuManagerWordAtNine(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id);
LegoS32 __fastcall GetMenuManagerCharacterChoice(
	MenuManager* p_manager,
	void* p_unusedEdx,
	LegoU32 p_id,
	MenuCharacterPreviewChoice* p_output
);
LegoS32 __fastcall GetMenuManagerCarChoice(MenuManager* p_manager, void* p_unusedEdx, LegoU32 p_id, LegoU32* p_output);
void __fastcall ApplyMenuLanguage(SecondaryManager* p_manager, void* p_unusedEdx, LegoU32 p_mode);
LegoU32 __fastcall UpdateMenuResolution(SecondaryManager* p_manager, void* p_unusedEdx, const LegoU32* p_mode);
void __fastcall ApplyMenuGraphicsQuality(
	DisplayRenderer* p_renderer,
	void* p_unusedEdx,
	LegoS32 p_detail,
	LegoS32 p_geometryQuality,
	LegoS32 p_textureQuality
);

#endif // LR2_MENUMANAGERCURRENTACTION_H
