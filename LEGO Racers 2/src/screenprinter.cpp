#include "screenprinter.h"

#include "bitmapfontset.h"
#include "gamecontroller.h"
#include "gameobjectdispatch.h"
#include "globaloverlaymanager.h"

#include <new.h>

static void __fastcall InitializeScreenPrinter(GameObject* p_object, void*)
{
	((ScreenPrinter*) p_object)->Initialize();
}
static void __fastcall RemoveScreenPrinter(GameObject* p_object, void*)
{
	((ScreenPrinter*) p_object)->Remove();
}
static LegoU32 __fastcall UpdateScreenPrinter(GameObject* p_object, void*, LegoU32 p_elapsed)
{
	return ((ScreenPrinter*) p_object)->Update(p_elapsed);
}

// FUNCTION: LEGORACERS2 0x00410380
LegoU32 __fastcall GetScreenPrinterSize(GameObject*, void*)
{
	return sizeof(ScreenPrinter);
}

// Retail table5325b8: all 21 slots are supplied, including the size slot.
const ScreenPrinterDispatch g_screenPrinterDispatch = {
	{&GameObjectBaseDestroy,      &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &InitializeScreenPrinter,    &GameObjectBaseZero,
	 &UpdateScreenPrinter,        &RemoveScreenPrinter,
	 &GameObjectBaseZero,         &GameObjectBaseZero,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	 &GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument},
	&GetScreenPrinterSize
};

GameObject* __fastcall ConstructScreenPrinter(void* p_storage, void*, GameObjectTemplate* p_template)
{
	return new (p_storage) ScreenPrinter(p_template);
}

// STUB: LEGORACERS2 0x0045f770
ScreenPrinter::ScreenPrinter(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_vtable = &g_screenPrinterDispatch.m_base;
}

// STUB: LEGORACERS2 0x0045f7a0
void ScreenPrinter::Initialize()
{
	GameObject::Initialize();
	union FloatBits {
		LegoFloat m_float;
		LegoU32 m_word;
	} value;
	LegoU32 parameters[13];
	parameters[0] = 0;
	parameters[1] = (LegoU32) -1;
	parameters[4] = parameters[5] = parameters[8] = 0;
	value.m_float = 1.0f;
	parameters[6] = parameters[7] = parameters[10] = value.m_word;
	parameters[9] = 14;
	parameters[11] = parameters[12] = 0;
	LegoFloat y = 0.0f;
	for (LegoU32 i = 0; i < 4; ++i) {
		value.m_float = 0.1f;
		parameters[2] = value.m_word;
		value.m_float = y + 0.7f;
		parameters[3] = value.m_word;
		m_records[i] = g_globalOverlayManager.AllocateRecord(parameters, -1);
		y += 0.04f;
	}
}

// FUNCTION: LEGORACERS2 0x0045f870
void ScreenPrinter::Remove()
{
	for (LegoS32 i = 3; i >= 0; --i) {
		if (m_records[i] != NULL) {
			m_records[i]->Release();
			m_records[i] = NULL;
		}
	}
	GameObject::Remove();
}

// STUB: LEGORACERS2 0x0045f8a0
LegoU32 ScreenPrinter::Update(LegoU32)
{
	if (g_gameController.m_fontSet->m_fonts[0] == NULL) {
		for (LegoU32 i = 0; i < 4; ++i) {
			ClearText(i);
		}
		return 1;
	}
	return 0;
}

// STUB: LEGORACERS2 0x0045f8d0
void ScreenPrinter::ApplyFont()
{
	BitmapFont* font = g_gameController.m_fontSet->m_fonts[0];
	for (LegoU32 i = 0; i < 4; ++i) {
		if (m_records[i] != NULL) {
			m_records[i]->SetFont(font);
		}
	}
}

// STUB: LEGORACERS2 0x0045f900
void ScreenPrinter::SetText(const LegoChar* p_text, LegoU32 p_index)
{
	if (g_gameController.m_fontSet->m_fonts[0] != NULL) {
		m_records[p_index]->InitializeNarrowText(p_text, 0, 0.0f);
	}
}

// FUNCTION: LEGORACERS2 0x0045f930
void ScreenPrinter::ClearText(LegoU32 p_index)
{
	if (m_records[p_index] != NULL) {
		m_records[p_index]->InitializeTextByIndex(-1, 0, 0.0f);
	}
}
