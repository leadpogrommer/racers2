#include "charactervisualbehavior.h"

#include <math.h>

struct CharacterTrailPoint {
	Vector3 m_position;
	LegoFloat m_distance;
	Vector3 m_edge;
	LegoFloat m_lifetime;
};
struct CharacterTrail {
	CharacterTrailPoint m_points[16];
	LegoU32 m_state;
	LegoS16 m_count;
	LegoU16 m_style;
	LegoFloat m_textureOffset;
	LegoFloat m_secondaryOffset;
	LegoU32 m_unk0x210;
	GameLifecycleComponent* m_owner;
	CharacterTrail* m_previous;
	CharacterTrail* m_next;
};
struct CharacterTrailManager {
	LegoU32 m_unk0x00;
	LegoS32 m_count;
	CharacterTrail m_trails[32];
	undefined m_gap0x4408[0x443c - 0x4408];
	LegoU32 m_dirty;
};
DECOMP_SIZE_ASSERT(CharacterTrailPoint, 0x20);
DECOMP_SIZE_ASSERT(CharacterTrail, 0x220);
DECOMP_STATIC_ASSERT(offsetof(CharacterTrailManager, m_dirty) == 0x443c);

static void __fastcall StartCharacterFootstepTrail(
	CharacterTrailManager*,
	void*,
	const Vector3*,
	const Vector3*,
	GameLifecycleComponent*,
	LegoU32,
	CharacterTrail*
);
static void __fastcall ExtendCharacterFootstepTrail(
	CharacterTrailManager*,
	void*,
	const Vector3*,
	const Vector3*,
	LegoU32,
	CharacterTrail*
);
static LegoBool32 __fastcall IsCharacterTrailDirectionTooSteep(CharacterTrailManager*, void*, Vector3);
static void __fastcall UpdateCharacterTrailEdge(CharacterTrailManager*, void*, const Vector3*, CharacterTrail*);

// STUB: LEGORACERS2 0x004766c0
void __fastcall AddCharacterFootstepTrail(
	void* p_receiver,
	void*,
	const Vector3* p_position,
	const Vector3* p_direction,
	GameLifecycleComponent* p_owner,
	LegoU32 p_style
)
{
	CharacterTrailManager* manager = (CharacterTrailManager*) p_receiver;
	manager->m_dirty = 1;
	CharacterTrail* previous = NULL;
	for (LegoS32 i = 0; i < manager->m_count; ++i) {
		CharacterTrail* trail = &manager->m_trails[i];
		if (trail->m_state == 1 && trail->m_owner == p_owner) {
			Vector3 delta = *p_position;
			delta.Subtract(&trail->m_points[trail->m_count - 1].m_position);
			if (delta.Dot(&delta) <= 25.0f) {
				if (trail->m_count != 16) {
					ExtendCharacterFootstepTrail(manager, NULL, p_position, p_direction, p_style, trail);
					return;
				}
				trail->m_state = 2;
				previous = trail;
			}
			else {
				trail->m_state = 2;
			}
			break;
		}
	}
	StartCharacterFootstepTrail(manager, NULL, p_position, p_direction, p_owner, p_style, previous);
}

// STUB: LEGORACERS2 0x004767c0
static void __fastcall StartCharacterFootstepTrail(
	CharacterTrailManager* p_manager,
	void*,
	const Vector3* p_position,
	const Vector3* p_direction,
	GameLifecycleComponent* p_owner,
	LegoU32 p_style,
	CharacterTrail* p_previous
)
{
	if (p_manager->m_count == 32) {
		return;
	}
	CharacterTrail* trail = &p_manager->m_trails[p_manager->m_count++];
	trail->m_state = 1;
	trail->m_style = (p_style != 1) + 1;
	trail->m_owner = p_owner;
	trail->m_textureOffset = trail->m_secondaryOffset = 0.0f;
	trail->m_unk0x210 = 0;
	trail->m_previous = p_previous;
	trail->m_next = NULL;
	CharacterTrailPoint* point = &trail->m_points[0];
	if (p_previous != NULL) {
		point = &trail->m_points[1];
		trail->m_textureOffset = p_previous->m_textureOffset;
		trail->m_secondaryOffset = p_previous->m_secondaryOffset;
		p_previous->m_next = trail;
		for (LegoS32 i = 1; i < p_previous->m_count; ++i) {
			trail->m_textureOffset = 0.25f * p_previous->m_points[i].m_distance + trail->m_textureOffset;
			trail->m_secondaryOffset = 0.05f * p_previous->m_points[i].m_distance + trail->m_secondaryOffset;
		}
		trail->m_textureOffset = (LegoFloat) fmod(trail->m_textureOffset, 1.0);
	}
	point->m_position = *p_position;
	point->m_lifetime = 3.0f;
	if (p_style != 0) {
		point->m_lifetime *= 0.5f;
	}
	if (p_previous != NULL) {
		trail->m_count = 2;
		const CharacterTrailPoint* last = &p_previous->m_points[p_previous->m_count - 1];
		trail->m_points[0] = *last;
		Vector3 delta = *p_position;
		delta.Subtract(&last->m_position);
		if (IsCharacterTrailDirectionTooSteep(p_manager, NULL, delta) == 1) {
			--p_manager->m_count;
			return;
		}
		LegoFloat length = (LegoFloat) sqrt(delta.Dot(&delta));
		if (length > 3.0f) {
			delta.Scale(3.0f / length);
			point->m_position = delta;
			point->m_position.Add(&last->m_position);
			point->m_distance = 3.0f;
			UpdateCharacterTrailEdge(p_manager, NULL, p_direction, trail);
			AddCharacterFootstepTrail(p_manager, NULL, p_position, p_direction, p_owner, p_style);
			return;
		}
		point->m_distance = length;
		UpdateCharacterTrailEdge(p_manager, NULL, p_direction, trail);
	}
	else {
		trail->m_count = 1;
		point->m_distance = 0.0f;
	}
}

// STUB: LEGORACERS2 0x00476a90
static void __fastcall ExtendCharacterFootstepTrail(
	CharacterTrailManager* p_manager,
	void*,
	const Vector3* p_position,
	const Vector3* p_direction,
	LegoU32 p_style,
	CharacterTrail* p_trail
)
{
	LegoS32 count = p_trail->m_count;
	if (count > 1) {
		CharacterTrailPoint* previous = &p_trail->m_points[count - 2];
		Vector3 delta = *p_position;
		delta.Subtract(&previous->m_position);
		LegoFloat length = (LegoFloat) sqrt(delta.Dot(&delta));
		if (length > 0.01f && length < 3.0f) {
			if (IsCharacterTrailDirectionTooSteep(p_manager, NULL, delta) != 0) {
				return;
			}
			CharacterTrailPoint* last = previous + 1;
			last->m_position = *p_position;
			last->m_distance = length;
			last->m_lifetime = 3.0f;
			if (p_style != 0) {
				last->m_lifetime *= 0.5f;
			}
			UpdateCharacterTrailEdge(p_manager, NULL, p_direction, p_trail);
			return;
		}
	}
	CharacterTrailPoint* point = &p_trail->m_points[count];
	Vector3 delta = *p_position;
	delta.Subtract(&point[-1].m_position);
	if (IsCharacterTrailDirectionTooSteep(p_manager, NULL, delta) == 1) {
		return;
	}
	point->m_position = *p_position;
	point->m_lifetime = 3.0f;
	if (p_style != 0) {
		point->m_lifetime *= 0.5f;
	}
	LegoFloat length = (LegoFloat) sqrt(delta.Dot(&delta));
	if (length >= 0.01f) {
		++p_trail->m_count;
		if (length > 3.0f) {
			delta.Scale(3.0f / length);
			point->m_position = delta;
			point->m_position.Add(&point[-1].m_position);
			point->m_distance = 3.0f;
			UpdateCharacterTrailEdge(p_manager, NULL, p_direction, p_trail);
			AddCharacterFootstepTrail(p_manager, NULL, p_position, p_direction, p_trail->m_owner, p_style);
			return;
		}
		point->m_distance = length;
		UpdateCharacterTrailEdge(p_manager, NULL, p_direction, p_trail);
	}
}

// STUB: LEGORACERS2 0x00476d40
static LegoBool32 __fastcall IsCharacterTrailDirectionTooSteep(CharacterTrailManager*, void*, Vector3 p_delta)
{
	LegoFloat square = p_delta.m_x * p_delta.m_x + p_delta.m_z * p_delta.m_z + p_delta.m_y * p_delta.m_y;
	return square > 0.0001f && 0.75f < (LegoFloat) fabs(p_delta.m_y * (1.0f / (LegoFloat) sqrt(square)));
}

// STUB: LEGORACERS2 0x00476dd0
static void __fastcall UpdateCharacterTrailEdge(CharacterTrailManager*, void*, const Vector3*, CharacterTrail* p_trail)
{
	CharacterTrailPoint* point = &p_trail->m_points[p_trail->m_count - 1];
	if (point->m_distance <= 0.01f) {
		point->m_edge.m_x = point->m_edge.m_y = point->m_edge.m_z = 0.0f;
	}
	else {
		LegoFloat inverse = 1.0f / point->m_distance;
		point->m_edge.m_y = 0.0f;
		point->m_edge.m_z = (point->m_position.m_x - point[-1].m_position.m_x) * inverse - 0.0f;
		point->m_edge.m_x = 0.0f - (point->m_position.m_z - point[-1].m_position.m_z) * inverse;
		point->m_edge.m_y = 0.0f;
		LegoFloat scale = (LegoFloat) sqrt(
							  point->m_edge.m_z * point->m_edge.m_z + point->m_edge.m_y * point->m_edge.m_y +
							  point->m_edge.m_x * point->m_edge.m_x
						  ) *
						  0.4f;
		point->m_edge.Scale(scale);
		if (p_trail->m_count == 2) {
			point[-1].m_edge = point->m_edge;
		}
	}
}
