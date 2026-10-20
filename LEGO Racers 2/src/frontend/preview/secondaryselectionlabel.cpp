#include "frontend/preview/secondaryselectionlabel.h"

#include "frontend/overlay/globaloverlaymanager.h"

// STUB: LEGORACERS2 0x00463870
void* __fastcall ConstructSecondarySelectionLabel(void* p_object, void*)
{
	SecondarySelectionLabel* label = (SecondarySelectionLabel*) p_object;
	label->m_record = NULL;
	label->m_scale = 1.0f;
	return p_object;
}

// STUB: LEGORACERS2 0x00463880
void __fastcall LoadSecondarySelectionLabel(void* p_object, void*)
{
	SecondarySelectionLabel* label = (SecondarySelectionLabel*) p_object;
	LegoU32 parameters[13];
	parameters[0] = 0;
	parameters[1] = 0xffffffff;
	// Position is filled when the label receives its text; retail leaves these
	// two initial parameter words untouched.
	parameters[4] = 0;
	parameters[5] = 0;
	parameters[6] = 0x3f800000;
	parameters[7] = 0x3f800000;
	parameters[8] = 0;
	parameters[9] = 0;
	parameters[10] = 0x3f800000;
	parameters[11] = 0;
	parameters[12] = 0;
	label->m_record = g_globalOverlayManager.AllocateRecord(parameters, -1);
	label->m_width = 0.0f;
}

// STUB: LEGORACERS2 0x00463900
void __fastcall ReleaseSecondarySelectionLabel(void* p_object, void*)
{
	SecondarySelectionLabel* label = (SecondarySelectionLabel*) p_object;
	if (label->m_record != NULL) {
		label->m_record->Release();
		label->m_record = NULL;
	}
}

// STUB: LEGORACERS2 0x004639a0
void __fastcall SetSecondarySelectionLabelFont(void* p_object, void*, BitmapFont* p_font, LegoFloat p_scale)
{
	SecondarySelectionLabel* label = (SecondarySelectionLabel*) p_object;
	label->m_record->SetFont(p_font);
	label->m_record->SetScale(p_scale);
	label->m_scale = p_scale;
}
