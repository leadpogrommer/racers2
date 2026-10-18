#include "globaloverlaymanager.h"

#include "gameobjectrenderprep.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererquaddraw.h"
#include "rendererrecords.h"

#include <string.h>

namespace
{

typedef void(__fastcall* DrawOverlayRecord)(void*, void*, RendererDefaultRecord*);

void InitializeOverlayDefaults(RendererDefaultRecord* p_defaults)
{
	p_defaults->m_unk0x00 = 1;
	p_defaults->m_unk0x04 = 1;
	p_defaults->m_unk0x06 = 0;
	p_defaults->m_unk0x07 = 1;

	p_defaults->m_slots[0].m_value = 0;
	p_defaults->m_slots[0].m_mask = 0;
	p_defaults->m_slots[0].m_kind = 0;
	p_defaults->m_slots[0].m_flags = 0;
	for (LegoU32 index = 1; index < 4; ++index) {
		p_defaults->m_slots[index].m_value = 0xffffffff;
		p_defaults->m_slots[index].m_mask = 0xffff;
		p_defaults->m_slots[index].m_kind = 0x0f;
		p_defaults->m_slots[index].m_flags = 3;
	}
}

LegoBool32 MatchesOverlayType(void* p_record, LegoS32 p_currentType)
{
	LegoS32 recordType = *(LegoS32*) ((LegoU8*) p_record + 0x50);
	if (recordType == 3) {
		return *(LegoU32*) ((LegoU8*) g_unk0x00679ef0 + 0x1cc) != 0 ? TRUE : FALSE;
	}
	return recordType == p_currentType ? TRUE : FALSE;
}

void DrawOverlayRectangle(const LegoFloat* p_rectangle, RendererDefaultRecord* p_defaults)
{
	RendererGuiRectangle* rectangle = (RendererGuiRectangle*) p_rectangle;
	((const RendererMainDispatch*) g_displayRenderer->m_vtable)
		->m_drawGuiRectangle(g_displayRenderer, NULL, rectangle, p_defaults);
}

void DrawOverlayObject(void* p_record, LegoS32 p_currentType, RendererDefaultRecord* p_defaults)
{
	if (MatchesOverlayType(p_record, p_currentType) == FALSE) {
		return;
	}

	LegoU8* record = (LegoU8*) p_record;
	if ((record[0] & 2) == 0) {
		return;
	}

	const LegoFloat* source = (const LegoFloat*) (record + 4);
	// The last two FCOM branches also accept unordered comparisons.
	if (!(source[2] >= source[4] && source[3] >= source[5] && !(source[0] > source[6]) && !(source[1] > source[7]))) {
		return;
	}

	p_defaults->m_unk0x04 = *(const LegoU16*) (record + 0x30);
	if (*(LegoU32*) (record + 0x4c) != 2) {
		DrawOverlayRectangle(source, p_defaults);
		return;
	}

	LegoFloat tiledRectangle[21];
	memcpy(tiledRectangle, source, sizeof(tiledRectangle));
	LegoFloat width = tiledRectangle[8];
	LegoFloat height = tiledRectangle[9];
	LegoFloat finalX = tiledRectangle[2] - width * 0.5f;
	LegoFloat finalY = tiledRectangle[3] - height * 0.5f;
	for (LegoFloat x = source[0]; x < finalX; x += width) {
		for (LegoFloat y = source[1]; y < finalY; y += height) {
			tiledRectangle[2] = x + width;
			tiledRectangle[3] = y + height;
			tiledRectangle[0] = x;
			tiledRectangle[1] = y;
			DrawOverlayRectangle(tiledRectangle, p_defaults);
		}
	}
}

void DrawOverlayEntry(void* p_record, LegoS32 p_currentType, RendererDefaultRecord* p_defaults)
{
	LegoU8* record = (LegoU8*) p_record;
	LegoS32 recordType = *(LegoS32*) (record + 0x238);
	LegoBool32 typeMatches = recordType == 3 ? (*(LegoU32*) ((LegoU8*) g_unk0x00679ef0 + 0x1cc) != 0 ? TRUE : FALSE)
											 : (recordType == p_currentType ? TRUE : FALSE);
	if (typeMatches == FALSE || (record[0x204] & 2) == 0 || *(LegoU16*) (record + 4) == 0) {
		return;
	}
	const void* const* vtable = *(const void* const**) record;
	((DrawOverlayRecord) vtable[2])(p_record, NULL, p_defaults);
}

} // namespace

// STUB: LEGORACERS2 0x004b3540
void __fastcall RenderGlobalOverlayPass(GlobalOverlayManager* p_context, void*, LegoU32 p_mode)
{
	LegoS32 count = p_context->m_recordListCount;
	LegoS32 firstBand = 0;
	LegoS32 lastBand = 15 - count;
	if (p_mode != 0) {
		if (count == 0) {
			return;
		}
		firstBand = 16 - count;
		lastBand = 15;
	}

	LegoS32 currentType = *(LegoS32*) ((LegoU8*) g_unk0x00679ef0 + 0x1c8);
	if (currentType == -1) {
		return;
	}

	RendererDefaultRecord defaults;
	InitializeOverlayDefaults(&defaults);
	RendererMainDispatch* dispatch = (RendererMainDispatch*) g_displayRenderer->m_vtable;
	dispatch->m_applyTextureOperation(g_displayRenderer, NULL, 9);
	dispatch->m_setDepthState(g_displayRenderer, NULL, 0);

	if (firstBand <= lastBand) {
		for (LegoS32 band = firstBand; band <= lastBand; ++band) {
			GlobalOverlayListNode* firstList = (GlobalOverlayListNode*) p_context->m_overlayHeads[band];
			LegoS32 firstListCount = 0;
			while (firstList != NULL && firstListCount < 300) {
				DrawOverlayObject(firstList->m_value, currentType, &defaults);
				firstList = firstList->m_next;
				++firstListCount;
			}

			GlobalOverlayListNode* secondList = (GlobalOverlayListNode*) p_context->m_recordHeads[band];
			LegoS32 secondListCount = 0;
			while (secondList != NULL && secondListCount < 200) {
				DrawOverlayEntry(secondList->m_value, currentType, &defaults);
				secondList = secondList->m_next;
				++secondListCount;
			}
		}
	}

	dispatch->m_applyTextureOperation(g_displayRenderer, NULL, 10);
	dispatch->m_setDepthState(g_displayRenderer, NULL, 1);
}
