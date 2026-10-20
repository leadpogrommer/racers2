#include "frontend/input/menuinputbindings.h"
#include "frontend/menu/menumanagerresources.h"
#include "frontend/menu/menunetworkstatus.h"
#include "frontend/preview/menucarpreviewselector.h"
#include "frontend/preview/menucharacterpreviewselector.h"
#include "frontend/widgets/menuintegerselector.h"
#include "frontend/widgets/menulabelselector.h"
#include "frontend/widgets/menurangeslider.h"
#include "frontend/widgets/menutextentry.h"
#include "frontend/widgets/menutoggle.h"
#include "frontend/widgets/menutrackselector.h"
#include "game/controller/gamecontroller.h"
#include "memory/memoryheap.h"
#include "render/fonts/bitmapfontset.h"

#include <new.h>

namespace
{
void AssignFrontendWidget(MenuManager* p_manager, LegoS32 p_index, MenuWidgetBase* p_widget, BitmapFont* p_font)
{
	p_manager->m_widgets[p_index] = p_widget;
	p_widget->m_vtable->m_setFont(p_widget, NULL, p_font);
}
} // namespace

// STUB: LEGORACERS2 0x0041c890
void __fastcall CreateFrontendMenuWidgets(MenuManager* p_manager, void*)
{
	BitmapFont* font = g_gameController.m_fontSet->m_fonts[2];
	void* storage;
	LegoS32 index;
	for (index = 3; index < 7; ++index) {
		storage = AllocateHeapMemory(sizeof(MenuRangeSlider));
		AssignFrontendWidget(p_manager, index, storage == NULL ? NULL : new (storage) MenuRangeSlider, font);
	}
	storage = AllocateHeapMemory(sizeof(MenuToggle));
	AssignFrontendWidget(p_manager, 7, storage == NULL ? NULL : new (storage) MenuToggle, font);
	storage = AllocateHeapMemory(sizeof(MenuIntegerSelector));
	AssignFrontendWidget(p_manager, 8, storage == NULL ? NULL : new (storage) MenuIntegerSelector, font);
	storage = AllocateHeapMemory(sizeof(MenuLabelSelector));
	AssignFrontendWidget(p_manager, 9, storage == NULL ? NULL : new (storage) MenuLabelSelector, font);
	storage = AllocateHeapMemory(sizeof(MenuCharacterPreviewSelector));
	AssignFrontendWidget(p_manager, 10, storage == NULL ? NULL : new (storage) MenuCharacterPreviewSelector, font);
	storage = AllocateHeapMemory(sizeof(MenuCarPreviewSelector));
	AssignFrontendWidget(p_manager, 11, storage == NULL ? NULL : new (storage) MenuCarPreviewSelector, font);
	storage = AllocateHeapMemory(sizeof(MenuTextEntry));
	AssignFrontendWidget(p_manager, 12, storage == NULL ? NULL : new (storage) MenuTextEntry, font);
	storage = AllocateHeapMemory(sizeof(MenuInputBindingWidget));
	AssignFrontendWidget(p_manager, 13, storage == NULL ? NULL : new (storage) MenuInputBindingWidget, font);
	for (index = 14; index < 17; ++index) {
		storage = AllocateHeapMemory(sizeof(MenuNetworkStatusWidget));
		AssignFrontendWidget(p_manager, index, storage == NULL ? NULL : new (storage) MenuNetworkStatusWidget, font);
	}
	storage = AllocateHeapMemory(sizeof(MenuTrackSelector));
	AssignFrontendWidget(p_manager, 17, storage == NULL ? NULL : new (storage) MenuTrackSelector, font);
}

// STUB: LEGORACERS2 0x0041cab0
void __fastcall DestroyFrontendMenuWidgets(MenuManager* p_manager, void*)
{
	for (LegoS32 index = 17; index >= 3; --index) {
		MenuWidgetBase* widget = p_manager->m_widgets[index];
		if (widget != NULL) {
			widget->m_vtable->m_unk0x08(widget, NULL);
			widget = p_manager->m_widgets[index];
			if (widget != NULL) {
				widget->RestoreBaseDispatch();
				FreeHeapMemory(widget);
			}
			p_manager->m_widgets[index] = NULL;
		}
	}
}
