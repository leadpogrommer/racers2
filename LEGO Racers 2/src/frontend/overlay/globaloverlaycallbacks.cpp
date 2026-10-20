#include "frontend/overlay/globaloverlayglyph.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "game/objects/gameobjectdispatch.h"
#include "memory/memoryheap.h"
#include "render/fonts/bitmapfont.h"

namespace
{

union WordFloatBits {
	LegoU32 m_word;
	LegoFloat m_float;
};

LegoU32 __fastcall GlobalOverlayManagerTrue(GameObject*, void*)
{
	return TRUE;
}

LegoU32 __fastcall GlobalOverlayManagerZero(GameObject*, void*)
{
	return 0;
}

void __fastcall GlobalOverlayManagerNoOp(GameObject*, void*)
{
}

void __fastcall GlobalOverlayManagerNoOpWithOneArgument(GameObject*, void*, LegoU32)
{
}

void __fastcall GlobalOverlayManagerNoOpWithTwoArguments(GameObject*, void*, LegoU32, LegoU32)
{
}

} // namespace

// The record table begins with the three text-record callbacks. The manager's
// GameObject-compatible dispatch is the suffix beginning at byte offset 0x0c.
// The two adjacent table views explain the otherwise overlapping retail table
// addresses 0x00535ca0 and 0x00535cac.
const GlobalOverlayRecordDispatch g_globalOverlayRecordDispatch = {
	&MeasureGlobalOverlayText,
	&AdvanceGlobalOverlayText,
	&DrawGlobalOverlayText,
	{
		&GlobalOverlayManagerDestroy,       &GameObjectBaseApplyChildTransform,
		&GlobalOverlayManagerNoOp,          &GlobalOverlayManagerNoOp,
		&GameObjectBaseInitialize,          &GlobalOverlayManagerTrue,
		&GlobalOverlayManagerUpdateRecords, &GameObjectBaseRemove,
		&GlobalOverlayManagerZero,          &GlobalOverlayManagerZero,
		&GlobalOverlayManagerNoOp,          &GlobalOverlayManagerNoOp,
		&GameObjectBaseDrawChild,           &GlobalOverlayManagerNoOpWithOneArgument,
		&GlobalOverlayManagerZero,          &GlobalOverlayManagerNoOpWithTwoArguments,
		&GlobalOverlayManagerZero,          &GameObjectBaseZeroTwoArguments,
		&GameObjectBaseUpdateRegion,        &GameObjectBaseNoOpOneArgument,
	}
};

// STUB: LEGORACERS2 0x004b3330
GameObject* __fastcall GlobalOverlayManagerDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	GlobalOverlayManager* manager = (GlobalOverlayManager*) p_object;
	for (LegoU32 index = 0; index < 200; ++index) {
		manager->m_records[index].m_vtable = &g_globalOverlayRecordDispatch;
	}
	p_object->~GameObject();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004b3370
LegoU32 __fastcall GlobalOverlayManagerUpdateRecords(GameObject* p_object, void*, LegoU32 p_argument)
{
	GlobalOverlayManager* manager = (GlobalOverlayManager*) p_object;
	WordFloatBits delta;
	delta.m_word = p_argument;
	for (LegoU32 index = 0; index < 200; ++index) {
		GlobalOverlayRecord* record = &manager->m_records[index];
		if ((record->m_flags & 1) != 0) {
			GlobalOverlayRecordDispatch* dispatch = (GlobalOverlayRecordDispatch*) record->m_vtable;
			dispatch->m_advanceTextAnimation(record, NULL, delta.m_float);
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x004b43d0
void __fastcall AdvanceGlobalOverlayText(GlobalOverlayRecord* p_record, void*, LegoFloat p_delta)
{
	LegoU32 flags = p_record->m_animationFlags;
	if ((flags & 0x70) == 0 || (p_record->m_flags & 2) == 0) {
		return;
	}

	p_record->m_animationTime += p_delta;
	if ((flags & 0x10) == 0 || p_record->m_text[p_record->m_characterIndex + 1] == 0 ||
		!(p_record->m_elapsed < p_record->m_animationTime)) {
		return;
	}

	for (;;) {
		LegoS32 index = p_record->m_characterIndex;
		if (p_record->m_text[index + 1] == 0) {
			return;
		}
		p_record->m_animationTime -= p_record->m_elapsed;
		p_record->m_text[index] = p_record->m_text[index + 1];
		p_record->m_characterIndex = index + 1;

		for (LegoS32 schedule = 0; schedule < p_record->m_textLength; ++schedule) {
			if (p_record->m_characterAnimationIndices[schedule] == (LegoU8) p_record->m_characterIndex) {
				--p_record->m_characterAnimationIndices[schedule];
			}
		}
		p_record->m_text[p_record->m_characterIndex] = 0;
		if (!(p_record->m_animationTime > p_record->m_elapsed)) {
			return;
		}
	}
}

// STUB: LEGORACERS2 0x004b44e0
LegoFloat __fastcall MeasureGlobalOverlayText(GlobalOverlayRecord* p_record, void*)
{
	BitmapFont* font = (BitmapFont*) p_record->m_font;
	LegoU32 flags = p_record->m_animationFlags;
	LegoFloat scale = (flags & 8) != 0 ? *(LegoFloat*) ((LegoU8*) p_record + 0x230) : 1.0f;
	LegoBool32 scaleBoth = (flags & 8) != 0 ? TRUE : FALSE;
	if (p_record->m_type != 0 && g_globalOverlayManager.m_unk0x24944 == 0) {
		scale += scale;
		scaleBoth = TRUE;
	}

	LegoU32 characterSchedule = p_record->m_textLength != 0 ? p_record->m_characterAnimationIndices[0] : 0xffffffff;
	LegoU32 characterIndex = 0;
	LegoU32 scheduleIndex = 0;
	LegoU32 textIndex = 0;
	LegoFloat width = 0.0f;
	GlobalOverlayGlyphReplacementManagerView* replacements =
		(GlobalOverlayGlyphReplacementManagerView*) g_globalOverlayManager.m_unk0x2494c;

	while (p_record->m_text[textIndex] != 0) {
		LegoFloat advance = 0.0f;
		LegoU32 matchedLength = 1;
		LegoU16 character = p_record->m_text[textIndex];
		if (characterIndex == characterSchedule) {
			++scheduleIndex;
			if (scheduleIndex < (LegoU32) p_record->m_textLength) {
				characterSchedule = p_record->m_characterAnimationIndices[scheduleIndex];
			}
			else {
				characterSchedule = 0xffffffff;
			}
			if (replacements != NULL) {
				advance =
					GetGlobalOverlayGlyphReplacementAdvance(replacements, NULL, (LegoU32) character - 0xd903, font);
			}
		}
		else if (character == 0x20) {
			advance = *(LegoFloat*) ((LegoU8*) font + 0x28) * font->m_scaleX;
		}
		else if ((flags & 4) != 0 && character >= '0' && character <= '9') {
			advance = (font->m_spacing + *(LegoFloat*) ((LegoU8*) font + 0x24)) * font->m_scaleX;
		}
		else {
			font->GetTextAdvance(&p_record->m_text[textIndex], &matchedLength, &advance);
		}

		if (scaleBoth != FALSE) {
			advance *= scale;
		}
		width += advance;
		++characterIndex;
		textIndex += matchedLength;
	}
	return width;
}
