#include "menuwidgetinitialization.h"

#include "controllereventrecords.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollersession.h"

#include <stdio.h>
#include <string.h>

// These are the exact public storage types defined by menuwidgetderived.cpp.
struct MenuWidgetAnimationTuning {
	LegoU32 m_unk0x00, m_unk0x04, m_unk0x08;
	LegoFloat m_markerRadius, m_unk0x10, m_unk0x14, m_unk0x18, m_unk0x1c;
};
struct MenuWidgetChoiceTuning {
	LegoFloat m_unk0x00, m_unk0x04, m_unk0x08, m_unk0x0c, m_unk0x10, m_unk0x14, m_unk0x18, m_unk0x1c;
};
extern MenuWidgetAnimationTuning g_menuWidgetAnimationTuning;
extern MenuWidgetChoiceTuning g_menuWidgetChoiceTuning;
extern LegoFloat g_menuWidgetLabelPositionTuning[2];
// GLOBAL: LEGORACERS2 0x00561a28
static LegoChar g_menuSessionScoreText[16];
static GlobalOverlayRecord* AllocateMenuWidgetRecord(BitmapFont* p_font)
{
	LegoU32 parameters[13];
	parameters[0] = (LegoU32) p_font;
	parameters[1] = -1;
	// Retail leaves initial position words 2 and 3 uninitialized.
	parameters[4] = parameters[5] = 0;
	parameters[6] = parameters[7] = 0x3f800000;
	parameters[8] = 0;
	parameters[9] = 9;
	parameters[10] = 0x3f800000;
	parameters[11] = parameters[12] = 0;
	return g_globalOverlayManager.AllocateRecord(parameters, -1);
}
static void MakeMenuWidgetSpriteParameters(LegoU32* p_parameters, LegoS32 p_list)
{
	memset(p_parameters, 0, 21 * 4);
	p_parameters[6] = p_parameters[7] = 0x3f800000;
	p_parameters[10] = -1;
	p_parameters[18] = 1;
	p_parameters[20] = p_list;
}

// STUB: LEGORACERS2 0x00419cb0
void __fastcall InitializeMenuVolumeControl(
	MenuVolumeSlider* p_widget,
	void*,
	LegoS32 p_initial,
	const LegoFloat* p_position,
	LegoFloat p_value,
	LegoU32 p_audioTargets,
	LegoFloat p_step
)
{
	p_widget->BeginState();
	if (!(p_step > 0.0f)) {
		p_step = 0.03225806355f;
	}
	p_widget->m_initialValue = p_initial;
	p_widget->m_value = p_value;
	p_widget->m_step = p_step;
	p_widget->m_audioTargets = p_audioTargets;
	p_widget->m_originX = p_position[0];
	p_widget->m_originY = p_position[1];
	p_widget->m_originY += g_menuWidgetChoiceTuning.m_unk0x04 * 0.5f;
	p_widget->m_valueLabel = AllocateMenuWidgetRecord(p_widget->m_font);
	p_widget->m_valueLabel->InitializeTextByIndex(0x1a, 1, 0.0f);
	LegoFloat labelPosition[2] = {
		p_widget->m_originX + g_menuWidgetLabelPositionTuning[0],
		p_widget->m_originY + g_menuWidgetLabelPositionTuning[1]
	};
	p_widget->m_valueLabel->SetPosition(labelPosition, 0xffffffff);
	LegoU32 parameters[21];
	MakeMenuWidgetSpriteParameters(parameters, 9);
	const ControllerEventRecordTable* resources = (const ControllerEventRecordTable*) g_gameController.m_unk0x90;
	const LegoFloat* offsets = (const LegoFloat*) &g_menuWidgetAnimationTuning;
	const LegoFloat* sizes = (const LegoFloat*) &g_menuWidgetChoiceTuning;
	for (LegoS32 index = 0; index < 4; ++index) {
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		p_widget->m_sprites[index] = sprite;
		LegoFloat position[2] = {
			p_widget->m_originX + offsets[index * 2],
			p_widget->m_originY + offsets[index * 2 + 1]
		};
		sprite->SetPosition(position);
		sprite->SetSize(sizes + index * 2);
		memcpy(&sprite->m_texture, &resources->m_records[164 + index], sizeof(ControllerEventRecord));
		sprite->SetVisible(1);
	}
	p_widget->m_sprites[1]->SetList(10);
	p_widget->UpdateMarker();
	LegoFloat center[2] = {p_widget->m_originX, p_widget->m_originY + 0.145f};
	p_widget->CreateChildSprites(center, p_widget->m_valueLabel->m_measuredWidth * 0.5f, -1, 1.0f);
	p_widget->m_vtable->m_updateBounds(p_widget, NULL);
}

// STUB: LEGORACERS2 0x0041a340
void __fastcall InitializeMenuRangeControl(
	MenuRangeSlider* p_widget,
	void*,
	LegoS32 p_initial,
	const LegoFloat* p_position,
	LegoFloat p_value,
	LegoFloat p_step,
	CharBuffer24* p_names,
	LegoS32 p_leftString,
	LegoS32 p_rightString,
	LegoFloat p_width
)
{
	p_widget->BeginState();
	p_widget->m_initialValue = p_initial;
	p_widget->m_value = p_value;
	p_widget->m_width = p_width;
	p_widget->m_step = p_step;
	p_widget->m_centerX = p_position[0];
	p_widget->m_centerY = p_position[1];
	p_widget->m_centerX += p_width * 0.5f;
	p_widget->m_centerY += g_menuRangeMarkerSize[1] * 0.5f;
	p_widget->m_displayedValue = p_value;
	p_widget->m_dragging = 0;
	p_widget->m_valueNames = p_names;
	p_widget->m_displayStep = (1.0f / 2.0f) * p_step;
	LegoU32 parameters[21];
	MakeMenuWidgetSpriteParameters(parameters, 0);
	const ControllerEventRecordTable* resources = (const ControllerEventRecordTable*) g_gameController.m_unk0x90;
	for (LegoS32 index = 0; index < 4; ++index) {
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		p_widget->m_sprites[index] = sprite;
		memcpy(&sprite->m_texture, &resources->m_records[171 + index], sizeof(ControllerEventRecord));
		sprite->SetList(index == 3 ? 10 : 9);
		sprite->SetVisible(1);
	}
	LegoFloat position[2] = {p_widget->m_centerX, p_widget->m_centerY};
	p_widget->m_sprites[1]->SetPosition(position);
	position[0] = p_widget->m_centerX - (p_widget->m_width - 0.009765625f) * 0.5f;
	p_widget->m_sprites[0]->SetPosition(position);
	position[0] += (p_widget->m_width - 0.009765625f);
	p_widget->m_sprites[2]->SetPosition(position);
	p_widget->m_sprites[3]->SetPosition(position);
	LegoFloat size[2] = {p_widget->m_width - (0.009765625f + 0.009765625f), g_menuRangeMarkerSize[1]};
	p_widget->m_sprites[1]->SetSize(size);
	size[0] = 0.009765625f;
	p_widget->m_sprites[0]->SetSize(size);
	p_widget->m_sprites[2]->SetSize(size);
	p_widget->m_sprites[3]->SetSize(g_menuRangeMarkerSize);
	p_widget->UpdateMarker();
	for (LegoS32 label = 0; label < 3; ++label) {
		p_widget->m_labels[label] = AllocateMenuWidgetRecord(p_widget->m_font);
	}
	p_widget->m_labels[0]->InitializeTextByIndex(p_leftString, 0, 0.0f);
	p_widget->m_labels[0]->SetScale(0.35f);
	position[0] = p_widget->m_centerX - p_widget->m_width * 0.5f;
	position[1] = g_menuRangeMarkerSize[1] * 0.5f + 0.0f + p_widget->m_centerY;
	p_widget->m_labels[0]->SetPosition(position, 0xffffffff);
	p_widget->m_labels[1]->InitializeTextByIndex(p_rightString, 2, 0.0f);
	p_widget->m_labels[1]->SetScale(0.35f);
	position[0] += p_widget->m_width;
	p_widget->m_labels[1]->SetPosition(position, 0xffffffff);
	p_widget->UpdateValueLabel();
	position[0] = p_widget->m_centerX;
	position[1] = p_widget->m_centerY;
	p_widget->CreateChildSprites(position, p_widget->m_width * 0.5f, -1, 1.0f);
	p_widget->m_vtable->m_updateBounds(p_widget, NULL);
}

// STUB: LEGORACERS2 0x0041acd0
void __fastcall InitializeMenuToggleControl(
	MenuToggle* p_widget,
	void*,
	LegoS32 p_initial,
	const LegoFloat* p_position,
	LegoS32 p_value,
	LegoU32 p_alignLeft,
	LegoU32 p_wrap
)
{
	p_widget->BeginState();
	p_widget->m_initialValue = p_initial;
	p_widget->m_unk0x14 = p_wrap;
	p_widget->m_value = p_value;
	p_widget->m_label = AllocateMenuWidgetRecord(p_widget->m_font);
	p_widget->m_label->SetPosition(p_position, 0xffffffff);
	p_widget->m_label->InitializeTextByIndex(p_value == 0, 1, 0.0f);
	p_widget->m_vtable->m_updateBounds(p_widget, NULL);
	LegoFloat right = p_widget->m_bounds[2], left = p_widget->m_bounds[0];
	LegoFloat position[2] = {p_position[0], p_position[1]};
	if (p_alignLeft == 0) {
		position[0] += (right - left) * 0.5f;
		p_widget->m_label->SetPosition(position, 0xffffffff);
	}
	position[1] += p_widget->m_label->GetLineHeight() * 0.5f;
	p_widget->CreateChildSprites(position, (right - left) * 0.5f, -1, 1.0f);
	p_widget->m_vtable->m_updateBounds(p_widget, NULL);
}

// STUB: LEGORACERS2 0x0041b8a0
void __fastcall InitializeMenuIntegerControl(
	MenuIntegerSelector* p_widget,
	void*,
	LegoS32 p_initial,
	const LegoFloat* p_position,
	LegoS32 p_value,
	LegoS32 p_minimum,
	LegoS32 p_maximum,
	LegoU32 p_wrap
)
{
	p_widget->BeginState();
	p_widget->m_unk0x14 = p_wrap;
	p_widget->m_value = p_value;
	p_widget->m_initialValue = p_initial;
	p_widget->m_maximum = p_maximum;
	p_widget->m_minimum = p_minimum;
	p_widget->m_label = AllocateMenuWidgetRecord(p_widget->m_font);
	p_widget->FormatNumber();
	p_widget->m_label->SetPosition(p_position, 0xffffffff);
	p_widget->m_label->InitializeNarrowText(p_widget->m_number, 1, 0.0f);
	if (p_widget->m_minimum < p_widget->m_maximum) {
		p_widget->m_vtable->m_updateBounds(p_widget, NULL);
		LegoFloat center[2] = {
			(p_widget->m_bounds[0] + p_widget->m_bounds[2]) * 0.5f,
			(p_widget->m_bounds[1] + p_widget->m_bounds[3]) * 0.5f
		};
		p_widget->CreateChildSprites(center, center[0] - p_widget->m_bounds[0], -1, 1.0f);
	}
	p_widget->m_vtable->m_updateBounds(p_widget, NULL);
}

// STUB: LEGORACERS2 0x00426460
void __fastcall InitializeMenuSessionTrackGroup(
	MenuTextGroup* p_widget,
	void*,
	LegoS32 p_initial,
	const LegoFloat* p_position
)
{
	p_widget->BeginState();
	p_widget->m_initialValue = p_initial;
	if (g_gameControllerSession->m_mode == 2) {
		LegoFloat position[2] = {p_position[0], p_position[1]};
		p_widget->m_records[0] = AllocateMenuWidgetRecord(p_widget->m_font);
		p_widget->m_records[0]->SetScale(1.0f);
		LegoS32 track = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a70),
				count = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a6c);
		p_widget->m_records[0]->InitializeTextByIndex(0x142 + (track < count), 1, 0.0f);
		p_widget->m_records[0]->SetPosition(position, 0xffffffff);
		for (LegoS32 player = 0; player < 2; ++player) {
			position[1] += 0.05f;
			position[0] = p_position[0] - 0.15f;
			GlobalOverlayRecord* name = AllocateMenuWidgetRecord(p_widget->m_font);
			p_widget->m_records[1 + player * 2] = name;
			name->SetScale(1.0f);
			name->InitializeText((const LegoU16*) ((LegoU8*) g_gameControllerSettings + 4 + player * 20), 1, 0.0f);
			name->SetPosition(position, 0xffffffff);
			position[0] = p_position[0] + 0.2f;
			GlobalOverlayRecord* score = AllocateMenuWidgetRecord(p_widget->m_font);
			p_widget->m_records[2 + player * 2] = score;
			score->SetScale(1.0f);
			LegoS32 firstScore = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a74);
			LegoS32 value =
				player == 0 ? firstScore : *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a70) - firstScore;
			sprintf(g_menuSessionScoreText, "%d", value);
			score->InitializeNarrowText(g_menuSessionScoreText, 4, 0.0f);
			score->SetPosition(position, 0xffffffff);
		}
	}
	p_widget->m_vtable->m_updateBounds(p_widget, NULL);
}
