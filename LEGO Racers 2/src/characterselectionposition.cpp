#include "characterselectionposition.h"

#include "charactervisualbehavior.h"
#include "renderer.h"
#include "rendererbufferdispatch.h"
#include "rendererconstructiondispatch.h"
#include "rendererrecords.h"

#include <float.h>
#include <string.h>

template <class T>
static T& SelectionField(void* p_object, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_object + p_offset);
}

// GLOBAL: LEGORACERS2 0x00571498
Vector3 g_characterBuilderPositionOffset;
// GLOBAL: LEGORACERS2 0x005715e8
Vector3 g_characterSelectionPositionOffset;
// GLOBAL: LEGORACERS2 0x005715f8
Vector3 g_characterSelectionFallbackPosition;

// STUB: LEGORACERS2 0x00401f80
static LegoBool32 __fastcall GetSelectedBuilderSeatPosition(
	void* p_builder,
	void*,
	Vector3* p_position,
	LegoU32 p_offset
)
{
	void* selection;
	if (SelectionField<LegoU32>(p_builder, 0x49c) != 0) {
		LegoU32 index = SelectionField<LegoU32>(p_builder, 0x4a0);
		selection = SelectionField<void*>(p_builder, 0x4a0 + index * 4);
	}
	else {
		if (SelectionField<LegoS32>(p_builder, 0x198) < 0) {
			return FALSE;
		}
		selection = SelectionField<void*>(p_builder, 0x19c);
	}
	LegoS32 group = SelectionField<LegoS32>(selection, 0xcc);
	LegoS32 item = SelectionField<LegoS32>(selection, 0xd0);
	void* models = SelectionField<void*>(p_builder, 0x1a0);
	LegoBool32 found = FindBuilderSeatPosition(models, NULL, group, item, p_position, 0);
	if (p_offset != 0 && found) {
		p_position->Add(&SelectionField<Vector3>(models, group * 0x13c + item * 0x28 + 0x2a4));
	}
	return found;
}

// FUNCTION: LEGORACERS2 0x004059a0
LegoBool32 __cdecl ContainsSeatPositionTag(const LegoChar* p_tag, const LegoChar* p_path)
{
	LegoS32 tagLength = strlen(p_tag);
	LegoS32 pathLength = strlen(p_path);
	LegoS32 start = 0;
	LegoS32 matched = 0;
	while (start < pathLength) {
		if (p_tag[matched] == p_path[start + matched]) {
			++matched;
		}
		else {
			start += matched + 1;
			matched = 0;
		}
		if (matched == tagLength) {
			return TRUE;
		}
	}
	return FALSE;
}

static LegoU32 __cdecl GetSeatPositionVertexCount(void*);

// STUB: LEGORACERS2 0x00405cf0
LegoBool32 __fastcall FindBuilderSeatPosition(
	void* p_models,
	void*,
	LegoS32 p_group,
	LegoS32 p_item,
	Vector3* p_position,
	LegoU32 p_offset
)
{
	p_position->m_x = p_position->m_y = p_position->m_z = 0.0f;
	LegoU32 groupOffset = p_group * 0x13c;
	if (SelectionField<LegoU32>(p_models, groupOffset + 0x284) != 0) {
		return FALSE;
	}
	void* model = SelectionField<void*>(p_models, groupOffset + p_item * 0x28 + 0x288);
	void* geometry = SelectionField<void*>(model, 0x0c);
	LegoBool32 found = FALSE;
	for (LegoS32 i = 0; i < SelectionField<LegoS32>(geometry, 8); ++i) {
		void* primitive = (LegoU8*) SelectionField<void*>(geometry, 0x10) + i * 0x3c;
		LegoS32 count = GetSeatPositionVertexCount(primitive);
		if (count != 0) {
			RendererVertexBuffer* vertices = SelectionField<RendererVertexBuffer*>(primitive, 0x0c);
			const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			found = TRUE;
			LegoFloat highest = -FLT_MAX;
			dispatch->m_lock(vertices, NULL, 0, count, 0, 0);
			dispatch->m_setCursor(vertices, NULL, 0);
			for (LegoS32 vertex = 0; vertex < count; ++vertex) {
				const Vector3* position = dispatch->m_getCurrentPosition(vertices, NULL);
				p_position->m_x += position->m_x;
				p_position->m_y += position->m_y;
				p_position->m_z = position->m_z + p_position->m_z;
				if (position->m_y > highest) {
					highest = position->m_y;
				}
				dispatch->m_advanceCursor(vertices, NULL);
			}
			dispatch->m_unlock(vertices, NULL, 0);
			p_position->m_y = highest;
			LegoFloat reciprocal = 1.0f / count;
			p_position->m_x *= reciprocal;
			p_position->m_z *= reciprocal;
			break;
		}
	}
	if (p_offset != 0 && found) {
		p_position->Add(&SelectionField<Vector3>(p_models, groupOffset + p_item * 0x28 + 0x2a4));
	}
	return found;
}

// STUB: LEGORACERS2 0x00405ed0
static LegoU32 __cdecl GetSeatPositionVertexCount(void* p_primitive)
{
	LegoS32 texture = -1;
	for (LegoS16 i = 0; i < SelectionField<LegoS16>(p_primitive, 0x18); ++i) {
		if (SelectionField<LegoU32>(p_primitive, 0x1c + i * 8) == 0) {
			texture = SelectionField<LegoS16>(p_primitive, 0x20 + i * 8);
			break;
		}
	}
	const RendererMainDispatch* renderer = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	RendererTextureRecord* record = renderer->m_getTextureRecord(g_displayRenderer, NULL, texture);
	if (ContainsSeatPositionTag("SEAT_POS", record->m_resourcePath)) {
		RendererVertexBuffer* vertices = SelectionField<RendererVertexBuffer*>(p_primitive, 0x0c);
		return ((const RendererVertexBufferDispatch*) vertices->m_vtable)->m_getCount(vertices, NULL);
	}
	return 0;
}

// STUB: LEGORACERS2 0x00461b10
static LegoU32 __fastcall GetBuilderSelectedAction(void* p_builder, void*, LegoU32* p_result)
{
	void* selection = SelectionField<void*>(p_builder, 0x1ec8);
	if (selection == NULL) {
		return 0;
	}
	p_result[2] = SelectionField<LegoU32>(selection, 0x3c);
	p_result[1] = SelectionField<LegoU32>(selection, 0x40);
	p_result[0] = SelectionField<LegoU32>(p_builder, 0x1ecc);
	p_result[3] = SelectionField<LegoU32>(selection, 0x28);
	p_result[4] = SelectionField<LegoU32>(selection, 0x2c);
	p_result[5] = SelectionField<LegoU32>(selection, 0x30);
	p_result[6] = SelectionField<LegoU32>(selection, 0x4c);
	return 1;
}

// STUB: LEGORACERS2 0x004674c0
static void InitializeCharacterSelectionFallbackPosition()
{
	g_characterSelectionFallbackPosition.m_x = g_characterSelectionFallbackPosition.m_y =
		g_characterSelectionFallbackPosition.m_z = 10000.0f;
}

// FUNCTION: LEGORACERS2 0x004674f0
static void InitializeCharacterBuilderPositionOffset()
{
	g_characterBuilderPositionOffset.m_x = 0.0f;
	g_characterBuilderPositionOffset.m_y = -10.0f;
	g_characterBuilderPositionOffset.m_z = 10.0f;
}

// FUNCTION: LEGORACERS2 0x00467760
static void InitializeCharacterSelectionPositionOffset()
{
	g_characterSelectionPositionOffset.m_x = 0.0f;
	g_characterSelectionPositionOffset.m_y = 1000.0f;
	g_characterSelectionPositionOffset.m_z = -2000.0f;
}

struct CharacterSelectionPositionStartup {
	CharacterSelectionPositionStartup()
	{
		InitializeCharacterSelectionFallbackPosition();
		InitializeCharacterBuilderPositionOffset();
		InitializeCharacterSelectionPositionOffset();
	}
};

// These retail CRT initializers must run before the builder loads: its
// constructor copies the builder offset before the position query is used.
static CharacterSelectionPositionStartup g_characterSelectionPositionStartup;

// STUB: LEGORACERS2 0x0046a100
void __fastcall GetCharacterSelectionPosition(void* p_selection, void*, Vector3* p_position)
{
	void* builder = SelectionField<void*>(p_selection, 0x20);
	LegoBool32 found;
	if ((SelectionField<LegoU32>(p_selection, 0x10) & 4) != 0) {
		LegoU32 action[7];
		GetBuilderSelectedAction((LegoU8*) p_selection + 0x490, NULL, action);
		void* item = (void*) action[5];
		if (action[0] == 0x0b && item != NULL) {
			LegoU32 index = SelectionField<LegoU32>(item, 0);
			found = FindBuilderSeatPosition(
				SelectionField<void*>(builder, 0x1a0),
				NULL,
				SelectionField<LegoS32>(item, index * 0x14 + 0x1c),
				SelectionField<LegoS32>(item, index * 0x14 + 0x20),
				p_position,
				1
			);
		}
		else {
			found = GetSelectedBuilderSeatPosition(builder, NULL, p_position, 1);
		}
		p_position->Add(&g_characterBuilderPositionOffset);
	}
	else {
		LegoS32 index = SelectionField<LegoS32>(p_selection, 0x30);
		if (index < 0) {
			*p_position = g_characterSelectionFallbackPosition;
			return;
		}
		void* item = SelectionField<void*>(p_selection, 0x34 + index * 4);
		if (item == NULL) {
			*p_position = g_characterSelectionFallbackPosition;
			return;
		}
		index = SelectionField<LegoU32>(item, 0);
		found = FindBuilderSeatPosition(
			SelectionField<void*>(builder, 0x1a0),
			NULL,
			SelectionField<LegoS32>(item, index * 0x14 + 0x1c),
			SelectionField<LegoS32>(item, index * 0x14 + 0x20),
			p_position,
			1
		);
		p_position->Add(&g_characterSelectionPositionOffset);
	}
	if (!found) {
		*p_position = g_characterSelectionFallbackPosition;
	}
}
