#include "reverbgeometry.h"

namespace
{
LegoFloat PlaneDistance(const ReverbBoundaryPlane* p_plane, const Vector3* p_point)
{
	return (p_plane->m_reference.m_y - p_point->m_y) * p_plane->m_normal.m_y +
		   (p_plane->m_reference.m_z - p_point->m_z) * p_plane->m_normal.m_z +
		   (p_plane->m_reference.m_x - p_point->m_x) * p_plane->m_normal.m_x;
}

LegoBool32 ZeroOrUnordered(LegoFloat p_value)
{
	return !(p_value < 0.0f || p_value > 0.0f);
}
} // namespace

// STUB: LEGORACERS2 0x0043d2d0
LegoBool32 ReverbBoundaryPlane::IntersectSegment(
	const Vector3* p_origin,
	const Vector3* p_direction,
	const Vector3* p_end,
	LegoFloat* p_distance
)
{
	if (m_mode == 1) {
		return FALSE;
	}
	Vector3 difference =
		{p_origin->m_x - m_reference.m_x, p_origin->m_y - m_reference.m_y, p_origin->m_z - m_reference.m_z};
	if (difference.Dot(&difference) > m_radiusSquared) {
		return FALSE;
	}
	LegoFloat first = PlaneDistance(this, p_origin);
	*p_distance = first;
	LegoFloat last = PlaneDistance(this, p_end);
	if (ZeroOrUnordered(last)) {
		return FALSE;
	}
	Vector3 intersection;
	if (ZeroOrUnordered(first)) {
		intersection = *p_origin;
		*p_distance = -last;
	}
	else {
		if ((first >= 0.0f || last < 0.0f) && (first <= 0.0f || last >= 0.0f)) {
			return FALSE;
		}
		LegoFloat fraction = first / (p_direction->m_x * m_normal.m_x + m_normal.m_y * p_direction->m_y +
									  m_normal.m_z * p_direction->m_z);
		intersection.m_x = fraction * p_direction->m_x + p_origin->m_x;
		intersection.m_y = fraction * p_direction->m_y + p_origin->m_y;
		intersection.m_z = fraction * p_direction->m_z + p_origin->m_z;
	}
	const Vector3* previous = &m_corners[3];
	for (LegoS32 index = 0; index < 4; ++index) {
		const Vector3* current = &m_corners[index];
		LegoFloat side = ((current->m_z - intersection.m_z) * (previous->m_y - intersection.m_y) -
						  (current->m_y - intersection.m_y) * (previous->m_z - intersection.m_z)) *
							 m_normal.m_x +
						 ((previous->m_z - intersection.m_z) * (current->m_x - intersection.m_x) -
						  (current->m_z - intersection.m_z) * (previous->m_x - intersection.m_x)) *
							 m_normal.m_y +
						 ((current->m_y - intersection.m_y) * (previous->m_x - intersection.m_x) -
						  (previous->m_y - intersection.m_y) * (current->m_x - intersection.m_x)) *
							 m_normal.m_z;
		if (!(side >= 0.0f)) {
			return FALSE;
		}
		previous = current;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x0043d5d0
LegoBool32 ReverbBoundaryPlane::ContainsPoint(const Vector3* p_point) const
{
	return m_mode == 2 || !(PlaneDistance(this, p_point) >= 0.0f);
}

// STUB: LEGORACERS2 0x0043d660
LegoBool32 ReverbVolumeView::ContainsPoint(const Vector3* p_point, LegoU32 p_underground) const
{
	if (m_underground != p_underground || m_planeCount == 0) {
		return FALSE;
	}
	for (LegoS32 index = 0; index < m_planeCount; ++index) {
		if (!m_planes[index]->ContainsPoint(p_point)) {
			return FALSE;
		}
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x0043d6c0
LegoBool32 ReverbVolumeView::IntersectSegment(
	const Vector3* p_origin,
	const Vector3* p_direction,
	const Vector3* p_end,
	LegoFloat* p_distance
)
{
	for (LegoS32 index = 0; index < m_planeCount; ++index) {
		if (m_planes[index]->IntersectSegment(p_origin, p_direction, p_end, p_distance)) {
			return TRUE;
		}
	}
	return FALSE;
}
