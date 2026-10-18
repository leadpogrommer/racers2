#include "overlayrecords.h"

#include "globaloverlaymanager.h"
#include "localizedstringtable.h"

// STUB: LEGORACERS2 0x004b3470
GlobalOverlayRecord* GlobalOverlayManager::AllocateRecord(const LegoU32* p_parameters, LegoS32 p_listId)
{
	GlobalOverlayListNode* node = m_recordNodeRoot;
	if (node == NULL) {
		return NULL;
	}

	GlobalOverlayRecord* record = (GlobalOverlayRecord*) node->m_value;
	MoveRecord(record, -1, (LegoS32) p_parameters[9]);
	record->InitializeFromParameters(p_parameters, p_listId);
	return record;
}

// STUB: LEGORACERS2 0x004b34b0
void GlobalOverlayManager::MoveRecord(GlobalOverlayRecord* p_record, LegoS32 p_sourceList, LegoS32 p_destinationList)
{
	GlobalOverlayListNode** sourceHead = (p_sourceList == -1) ? &m_recordNodeRoot : &m_recordHeads[p_sourceList];
	GlobalOverlayListNode* previous = NULL;
	GlobalOverlayListNode* node = *sourceHead;
	LegoU32 count = 0;

	while (node != NULL && count < 200) {
		if (node->m_value == p_record) {
			break;
		}
		++count;
		previous = node;
		node = node->m_next;
	}

	if (node != NULL) {
		if (previous != NULL) {
			previous->m_next = node->m_next;
		}
		else {
			*sourceHead = node->m_next;
		}

		GlobalOverlayListNode** destinationHead =
			(p_destinationList == -1) ? &m_recordNodeRoot : &m_recordHeads[p_destinationList];
		node->m_next = *destinationHead;
		*destinationHead = node;
	}
}

// STUB: LEGORACERS2 0x004b38c0
void GlobalOverlayRecord::InitializeFromParameters(const LegoU32* p_parameters, LegoS32 p_stringIndex)
{
	LegoU32* destination = (LegoU32*) &m_font;
	for (LegoU32 i = 0; i < 13; ++i) {
		destination[i] = p_parameters[i];
	}
	m_flags = 3;
	InitializeTextByIndex(p_stringIndex, m_animationFlags, m_elapsed);
	SetPosition(&m_positionX, m_animationFlags);
}

// STUB: LEGORACERS2 0x004b3920
void GlobalOverlayRecord::Release()
{
	m_flags = 0;
	m_text[0] = 0;
	g_globalOverlayManager.MoveRecord(this, m_listId, -1);
}

// STUB: LEGORACERS2 0x004b3980
void GlobalOverlayRecord::InitializeText(const LegoU16* p_text, LegoU32 p_flags, LegoFloat p_time)
{
	LegoU32 oldFlags = m_animationFlags;
	m_elapsed = p_time;
	m_animationFlags = p_flags;
	m_animationTime = 0.0f;
	m_characterIndex = 0;
	m_textLength = 0;
	m_flags |= 2;

	LegoU16* destination = m_text;
	if ((p_flags & 0x10) != 0) {
		m_text[0] = 0;
		++destination;
	}

	if ((p_flags & 0x20) != 0 && p_time != 0.0f) {
		const LegoFloat pi = 3.14159265358979323846f;
		m_elapsed = (pi + pi) / p_time;
	}

	LegoU32 sourceStep = 1;
	LegoBool32 finished = 0;
	void* glyphManager = (void*) g_globalOverlayManager.m_unk0x2494c;

	while (!finished) {
		sourceStep = 1;
		if (glyphManager != NULL && ExpandOverlayGlyphSequence(glyphManager, NULL, p_text, &sourceStep, &destination)) {
		}
		else {
			LegoS32 glyphIndex = -1;
			if (glyphManager != NULL) {
				glyphIndex = FindOverlayGlyphSequence(glyphManager, NULL, p_text, &sourceStep);
			}

			if (glyphIndex < 0) {
				if (*p_text != 10) {
					*destination++ = *p_text;
				}
			}
			else {
				m_characterAnimationIndices[m_textLength] = (LegoU8) (destination - m_text);
				++m_textLength;
				*destination++ = (LegoU16) (glyphIndex - 0x26fd);
			}
		}

		if (*p_text == 0) {
			finished = 1;
		}
		p_text += sourceStep;
	}

	LegoFloat position[2];
	position[0] = m_positionX;
	position[1] = m_positionY;
	if ((oldFlags & 1) != 0) {
		position[0] += m_measuredWidth * 0.5f;
	}
	else if ((oldFlags & 2) != 0) {
		position[0] += m_measuredWidth;
	}
	SetPosition(position, p_flags);
}

// STUB: LEGORACERS2 0x004b3b10
void GlobalOverlayRecord::InitializeTextByIndex(LegoS32 p_stringIndex, LegoU32 p_flags, LegoFloat p_time)
{
	if (p_stringIndex == -1) {
		const LegoU16 emptyText[1] = {0};
		InitializeText(emptyText, p_flags, p_time);
	}
	else {
		const LegoU16* text;
		if (GetLocalizedStringTable()->GetString(p_stringIndex, &text)) {
			InitializeText(text, p_flags, p_time);
		}
	}
}

// STUB: LEGORACERS2 0x004b3d40
void GlobalOverlayRecord::SetPosition(const LegoFloat* p_position, LegoU32 p_mode)
{
	m_positionX = p_position[0];
	m_positionY = p_position[1];
	if (p_mode != 0xffffffff) {
		m_animationFlags = (m_animationFlags & 0xfffffffc) | p_mode;
	}

	GlobalOverlayRecordDispatch* dispatch = (GlobalOverlayRecordDispatch*) m_vtable;
	LegoFloat width = dispatch->m_measureTextWidth(this, NULL);
	m_measuredWidth = width;

	if ((m_animationFlags & 1) != 0) {
		m_positionX -= width * 0.5f;
	}
	else if ((m_animationFlags & 2) != 0) {
		m_positionX -= width;
	}
}

// STUB: LEGORACERS2 0x004c4f30
LegoBool32 __fastcall CompareOverlayTextPrefix(
	void*,
	void*,
	const LegoU16* p_key,
	const LegoU16* p_text,
	LegoU32* p_matchedLength
)
{
	LegoU32 matchedLength = 0;
	while (*p_key != 0) {
		if (*p_key != *p_text) {
			return 0;
		}
		++p_key;
		++p_text;
		++matchedLength;
	}
	*p_matchedLength = matchedLength;
	return 1;
}

// STUB: LEGORACERS2 0x004c50b0
LegoS32 __fastcall FindOverlayGlyphSequence(void* p_glyphManager, void*, const LegoU16* p_key, LegoU32* p_matchedLength)
{
	const LegoU8* manager = (const LegoU8*) p_glyphManager;
	LegoS32 count = *(const LegoS32*) (manager + 0x54);
	const LegoU32* patterns = (const LegoU32*) (manager + 4);
	for (LegoS32 i = 0; i < count; ++i) {
		if (CompareOverlayTextPrefix(p_glyphManager, NULL, (const LegoU16*) patterns[i], p_key, p_matchedLength)) {
			return i;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x004c5110
LegoBool32 __fastcall ExpandOverlayGlyphSequence(
	void* p_glyphManager,
	void*,
	const LegoU16* p_key,
	LegoU32* p_unusedMatchedLength,
	LegoU16** p_output
)
{
	const LegoU8* manager = (const LegoU8*) p_glyphManager;
	LegoS32 count = *(const LegoS32*) (manager + 0x98);
	const LegoU32* patterns = (const LegoU32*) (manager + 0x58);
	for (LegoS32 i = 0; i < count; ++i) {
		const LegoU8* entry = (const LegoU8*) patterns[i];
		if (CompareOverlayTextPrefix(
				p_glyphManager,
				NULL,
				(const LegoU16*) patterns[i],
				p_key,
				p_unusedMatchedLength
			)) {
			const LegoU16* replacement = (const LegoU16*) (entry + 0x20);
			while (*replacement != 0) {
				*(*p_output)++ = *replacement++;
			}
			return 1;
		}
	}
	return 0;
}
