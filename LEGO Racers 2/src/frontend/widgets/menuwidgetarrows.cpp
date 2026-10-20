#include "frontend/resources/controllereventrecords.h"
#include "frontend/widgets/menuwidget.h"
#include "game/controller/gamecontroller.h"

#include <string.h>

// STUB: LEGORACERS2 0x00419780
void MenuWidgetBase::CreateChildSprites(
	const LegoFloat* p_center,
	LegoFloat p_halfWidth,
	LegoS32 p_listId,
	LegoFloat p_scale
)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	if (p_listId == -1) {
		p_listId = 9;
	}
	p_halfWidth += 0.03f;
	LegoFloat size[2] = {p_scale * 0.029296875f, p_scale * 0.0494791679084301f};
	LegoU32 parameters[21];
	parameters[0] = parameters[1] = parameters[2] = parameters[3] = 0;
	parameters[4] = parameters[5] = 0;
	parameters[6] = parameters[7] = 0x3f800000;
	parameters[10] = 0xffffffff;
	parameters[11] = 0;
	parameters[16] = parameters[17] = 0;
	parameters[18] = 1;
	parameters[19] = parameters[20] = 0;
	// Retail leaves tile size (8/9) and UV words (12..15) untouched here.
	// The texture region is copied from the controller immediately afterwards.
	for (LegoS32 index = 0; index < 2; ++index) {
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		m_childSprites[index] = sprite;
		LegoFloat position[2] = {index == 0 ? p_center[0] - p_halfWidth : p_center[0] + p_halfWidth, p_center[1]};
		sprite->SetPosition(position);
		sprite->SetSize(size);
		memcpy(&sprite->m_texture, &resources->m_records[168 + index], sizeof(ControllerEventRecord));
		sprite->SetList(p_listId);
		sprite->SetVisible(1);
	}
}
