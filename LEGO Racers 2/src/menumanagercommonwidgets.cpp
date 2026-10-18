#include "bitmapfontset.h"
#include "gamecontroller.h"
#include "memoryheap.h"
#include "menuchoiceselector.h"
#include "menumanagerresources.h"
#include "menutextgroup.h"
#include "menuvolumeslider.h"

#include <new.h>

// STUB: LEGORACERS2 0x0041c750
void __fastcall CreateCommonMenuWidgets(MenuManager* p_manager, void*)
{
	BitmapFont* font = g_gameController.m_fontSet->m_fonts[2];
	void* storage = AllocateHeapMemory(sizeof(MenuVolumeSlider));
	MenuWidgetBase* widget = storage == NULL ? NULL : new (storage) MenuVolumeSlider;
	p_manager->m_widgets[0] = widget;
	widget->m_vtable->m_setFont(widget, NULL, font);
	storage = AllocateHeapMemory(sizeof(MenuChoiceSelector));
	widget = storage == NULL ? NULL : new (storage) MenuChoiceSelector;
	p_manager->m_widgets[1] = widget;
	widget->m_vtable->m_setFont(widget, NULL, font);
	storage = AllocateHeapMemory(sizeof(MenuTextGroup));
	widget = storage == NULL ? NULL : new (storage) MenuTextGroup;
	p_manager->m_widgets[2] = widget;
	widget->m_vtable->m_setFont(widget, NULL, font);
	storage = AllocateHeapMemory(sizeof(MenuChoiceSelector));
	MenuChoiceSelector* extra = storage == NULL ? NULL : new (storage) MenuChoiceSelector;
	p_manager->m_extraChoiceSelector = extra;
	extra->m_vtable->m_setFont(extra, NULL, font);
}

// STUB: LEGORACERS2 0x0041c810
void __fastcall DestroyCommonMenuWidgets(MenuManager* p_manager, void*)
{
	MenuChoiceSelector* extra = p_manager->m_extraChoiceSelector;
	if (extra != NULL) {
		extra->m_vtable->m_unk0x08(extra, NULL);
		extra = p_manager->m_extraChoiceSelector;
		if (extra != NULL) {
			// Retail41b0c0 installs the derived table, then calls419730.
			extra->m_vtable = &g_menuChoiceSelectorDispatch;
			extra->RestoreBaseDispatch();
			FreeHeapMemory(extra);
		}
		p_manager->m_extraChoiceSelector = NULL;
	}
	for (LegoS32 i = 2; i >= 0; --i) {
		MenuWidgetBase* widget = p_manager->m_widgets[i];
		if (widget != NULL) {
			widget->m_vtable->m_unk0x08(widget, NULL);
			widget = p_manager->m_widgets[i];
			if (widget != NULL) {
				widget->RestoreBaseDispatch();
				FreeHeapMemory(widget);
			}
			p_manager->m_widgets[i] = NULL;
		}
	}
}
