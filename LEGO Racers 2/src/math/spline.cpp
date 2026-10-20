#include "math/spline.h"

#include <math.h>

// GLOBAL: LEGORACERS2 0x0067b6f8
LegoU8 g_splineCacheInitialized = 0;

// GLOBAL: LEGORACERS2 0x0067b79c
LegoFloat g_splineSampleInterval = 0.0f;

// FUNCTION length: 0x25f bytes (0x004d0f60-0x004d11be).
// STUB: LEGORACERS2 0x004d0f60
Vector3* SplinePath::FUN_004d0f60(Vector3* p_result, LegoS32 p_segmentIndex, LegoFloat p_t)
{
	LegoS32 nextIndex = p_segmentIndex + 1;
	if (nextIndex >= m_segmentCount) {
		nextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
	}

	SplineSegment* segment = m_segments + p_segmentIndex;
	if ((segment->m_flags & 1) != 0) {
		const Vector3& start = segment->m_vector0x00;
		const Vector3& end = m_segments[nextIndex].m_vector0x00;
		p_result->m_x = end.m_x;
		p_result->m_y = end.m_y;
		p_result->m_z = end.m_z;
		p_result->m_x -= start.m_x;
		p_result->m_y -= start.m_y;
		p_result->m_z -= start.m_z;
		p_result->m_x *= p_t;
		p_result->m_y *= p_t;
		p_result->m_z *= p_t;
		p_result->m_x += start.m_x;
		p_result->m_y += start.m_y;
		p_result->m_z += start.m_z;
		return p_result;
	}

	LegoS32 previousIndex;
	if (p_segmentIndex < 1) {
		previousIndex = m_closed != FALSE ? m_segmentCount - 1 : 0;
	}
	else {
		previousIndex = p_segmentIndex - 1;
	}

	LegoS32 nextNextIndex = nextIndex + 1;
	if (nextNextIndex >= m_segmentCount) {
		nextNextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
	}

	const LegoFloat(*basis)[4] = m_basis->m_basis;
	LegoFloat a = basis[0][0];
	LegoFloat b = basis[1][0];
	LegoFloat c = basis[2][0];
	LegoFloat d = basis[3][0];
	if (p_t != 0.0f) {
		LegoFloat t2 = p_t * p_t;
		LegoFloat t3 = t2 * p_t;
		a = t3 * basis[0][3] + t2 * basis[0][2] + p_t * basis[0][1] + a;
		b = t3 * basis[1][3] + t2 * basis[1][2] + p_t * basis[1][1] + b;
		c = t3 * basis[2][3] + t2 * basis[2][2] + p_t * basis[2][1] + c;
		d = t3 * basis[3][3] + t2 * basis[3][2] + p_t * basis[3][1] + d;
	}

	const Vector3& p0 = m_segments[previousIndex].m_vector0x00;
	const Vector3& p1 = segment->m_vector0x00;
	const Vector3& p2 = m_segments[nextIndex].m_vector0x00;
	const Vector3& p3 = m_segments[nextNextIndex].m_vector0x00;
	*p_result = p0;
	p_result->m_x = a * p_result->m_x;
	p_result->m_y = a * p_result->m_y;
	p_result->m_z = a * p_result->m_z;
	p_result->m_x += b * p1.m_x;
	p_result->m_y += b * p1.m_y;
	p_result->m_z += b * p1.m_z;
	p_result->m_x += c * p2.m_x;
	p_result->m_y += c * p2.m_y;
	p_result->m_z += c * p2.m_z;
	p_result->m_x += d * p3.m_x;
	p_result->m_y += d * p3.m_y;
	p_result->m_z += d * p3.m_z;
	return p_result;
}

// FUNCTION length: 0x1cd bytes (0x004d11c0-0x004d138c).
// STUB: LEGORACERS2 0x004d11c0
Vector3* SplinePath::FUN_004d11c0(Vector3* p_result, LegoS32 p_segmentIndex, LegoFloat p_t)
{
	LegoS32 nextIndex = p_segmentIndex + 1;
	if (nextIndex >= m_segmentCount) {
		nextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
	}

	SplineSegment* segment = m_segments + p_segmentIndex;
	if ((segment->m_flags & 1) != 0) {
		const Vector3& start = segment->m_vector0x00;
		const Vector3& end = m_segments[nextIndex].m_vector0x00;
		p_result->m_x = end.m_x;
		p_result->m_y = end.m_y;
		p_result->m_z = end.m_z;
		p_result->m_x -= start.m_x;
		p_result->m_y -= start.m_y;
		p_result->m_z -= start.m_z;
		return p_result;
	}

	LegoS32 previousIndex;
	if (p_segmentIndex < 1) {
		previousIndex = m_closed != FALSE ? m_segmentCount - 1 : 0;
	}
	else {
		previousIndex = p_segmentIndex - 1;
	}

	LegoS32 nextNextIndex = nextIndex + 1;
	if (nextNextIndex >= m_segmentCount) {
		nextNextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
	}

	const LegoFloat(*derivative)[4] = m_basis->m_derivative;
	LegoFloat t2 = p_t * p_t;
	LegoFloat a = t2 * derivative[0][3] + p_t * derivative[0][2] + derivative[0][1];
	LegoFloat b = t2 * derivative[1][3] + p_t * derivative[1][2] + derivative[1][1];
	LegoFloat c = t2 * derivative[2][3] + p_t * derivative[2][2] + derivative[2][1];
	LegoFloat d = t2 * derivative[3][3] + p_t * derivative[3][2] + derivative[3][1];

	const Vector3& p0 = m_segments[previousIndex].m_vector0x00;
	const Vector3& p1 = segment->m_vector0x00;
	const Vector3& p2 = m_segments[nextIndex].m_vector0x00;
	const Vector3& p3 = m_segments[nextNextIndex].m_vector0x00;
	*p_result = p0;
	p_result->m_x = a * p_result->m_x;
	p_result->m_y = a * p_result->m_y;
	p_result->m_z = a * p_result->m_z;
	p_result->m_x += b * p1.m_x;
	p_result->m_y += b * p1.m_y;
	p_result->m_z += b * p1.m_z;
	p_result->m_x += c * p2.m_x;
	p_result->m_y += c * p2.m_y;
	p_result->m_z += c * p2.m_z;
	p_result->m_x += d * p3.m_x;
	p_result->m_y += d * p3.m_y;
	p_result->m_z += d * p3.m_z;
	return p_result;
}

// FUNCTION length: 0x24c bytes (0x004d1390-0x004d15db).
// STUB: LEGORACERS2 0x004d1390
Vector3* SplinePath::FUN_004d1390(Vector3* p_result, LegoS32 p_segmentIndex, LegoFloat p_t)
{
	LegoS32 nextIndex = p_segmentIndex + 1;
	if (nextIndex >= m_segmentCount) {
		nextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
	}

	SplineSegment* segment = m_segments + p_segmentIndex;
	if ((segment->m_flags & 1) != 0) {
		const Vector3& start = segment->m_vector0x0c;
		const Vector3& end = m_segments[nextIndex].m_vector0x0c;
		p_result->m_x = end.m_x;
		p_result->m_y = end.m_y;
		p_result->m_z = end.m_z;
		p_result->m_x -= start.m_x;
		p_result->m_y -= start.m_y;
		p_result->m_z -= start.m_z;
		p_result->m_x *= p_t;
		p_result->m_y *= p_t;
		p_result->m_z *= p_t;
		p_result->m_x += start.m_x;
		p_result->m_y += start.m_y;
		p_result->m_z += start.m_z;
		return p_result;
	}

	LegoS32 previousIndex;
	if (p_segmentIndex < 1) {
		previousIndex = m_closed != FALSE ? m_segmentCount - 1 : 0;
	}
	else {
		previousIndex = p_segmentIndex - 1;
	}

	LegoS32 nextNextIndex = nextIndex + 1;
	if (nextNextIndex >= m_segmentCount) {
		nextNextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
	}

	const LegoFloat(*basis)[4] = m_basis->m_basis;
	LegoFloat t2 = p_t * p_t;
	LegoFloat t3 = t2 * p_t;
	LegoFloat a = t3 * basis[0][3] + t2 * basis[0][2] + p_t * basis[0][1] + basis[0][0];
	LegoFloat b = t3 * basis[1][3] + t2 * basis[1][2] + p_t * basis[1][1] + basis[1][0];
	LegoFloat c = t3 * basis[2][3] + t2 * basis[2][2] + p_t * basis[2][1] + basis[2][0];
	LegoFloat d = t3 * basis[3][3] + t2 * basis[3][2] + p_t * basis[3][1] + basis[3][0];

	const Vector3& p0 = m_segments[previousIndex].m_vector0x0c;
	const Vector3& p1 = segment->m_vector0x0c;
	const Vector3& p2 = m_segments[nextIndex].m_vector0x0c;
	const Vector3& p3 = m_segments[nextNextIndex].m_vector0x0c;
	*p_result = p0;
	p_result->m_x = a * p_result->m_x;
	p_result->m_y = a * p_result->m_y;
	p_result->m_z = a * p_result->m_z;
	p_result->m_x += b * p1.m_x;
	p_result->m_y += b * p1.m_y;
	p_result->m_z += b * p1.m_z;
	p_result->m_x += c * p2.m_x;
	p_result->m_y += c * p2.m_y;
	p_result->m_z += c * p2.m_z;
	p_result->m_x += d * p3.m_x;
	p_result->m_y += d * p3.m_y;
	p_result->m_z += d * p3.m_z;
	return p_result;
}

// FUNCTION length: 0xbc bytes (0x004d1800-0x004d18bb).
// STUB: LEGORACERS2 0x004d1800
void FUN_004d1800(const Vector3* p_reference, Vector3* p_adjustable)
{
	Vector3 original = *p_adjustable;

	LegoFloat xy = p_adjustable->m_y * p_reference->m_x - p_reference->m_y * p_adjustable->m_x;
	LegoFloat yz = p_adjustable->m_z * p_reference->m_y - p_adjustable->m_y * p_reference->m_z;
	LegoFloat zx = p_adjustable->m_x * p_reference->m_z - p_adjustable->m_z * p_reference->m_x;

	LegoFloat x = zx * p_reference->m_z - xy * p_reference->m_y;
	LegoFloat y = xy * p_reference->m_x - yz * p_reference->m_z;
	LegoFloat z = yz * p_reference->m_y - zx * p_reference->m_x;

	p_adjustable->m_x = x;
	p_adjustable->m_z = z;
	p_adjustable->m_y = y;

	if (x * x + y * y + p_adjustable->m_z * p_adjustable->m_z != 0.0f) {
		p_adjustable->Normalize();
	}
	else {
		*p_adjustable = original;
	}
}

// FUNCTION length: 0x4d bytes (0x004d1a20-0x004d1a6c).
// FUNCTION: LEGORACERS2 0x004d1a20
LegoFloat SplinePath::FUN_004d1a20()
{
	LegoFloat totalLength = 0.0f;
	LegoS32 segmentCount = m_segmentCount;
	if (m_closed == FALSE) {
		--segmentCount;
	}

	LegoS32 segmentIndex = 0;
	while (segmentIndex < segmentCount) {
		totalLength += FUN_004d1aa0(segmentIndex, 1.0f, 0.0f, 1);
		++segmentIndex;
	}

	m_totalLength = totalLength;
	return totalLength;
}

// FUNCTION length: 0x27 bytes (0x004d1a70-0x004d1a96).
// FUNCTION: LEGORACERS2 0x004d1a70
LegoFloat SplinePath::FUN_004d1a70()
{
	LegoFloat totalLength = 0.0f;
	LegoS32 segmentCount = m_segmentCount;
	if (m_closed == FALSE) {
		--segmentCount;
	}

	if (segmentCount > 0) {
		SplineSegment* segment = m_segments;
		do {
			totalLength += segment->m_length;
			++segment;
			--segmentCount;
		} while (segmentCount != 0);
	}

	m_totalLength = totalLength;
	return totalLength;
}

// FUNCTION length: 0x2eb bytes (0x004d1aa0-0x004d1d8a).
// STUB: LEGORACERS2 0x004d1aa0
LegoFloat SplinePath::FUN_004d1aa0(LegoS32 p_segmentIndex, LegoFloat p_t, LegoFloat p_startT, LegoS32 p_storeLength)
{
	LegoFloat totalLength = 0.0f;
	if (p_t == 0.0f) {
		if (p_storeLength != 0) {
			m_segments[p_segmentIndex].m_length = 0.0f;
			return totalLength;
		}
	}
	else {
		if (p_segmentIndex != m_cachedSegmentIndex) {
			m_cachedSampleIndex = -1;
		}
		m_cachedSegmentIndex = p_segmentIndex;
		SplineSegment* segment = m_segments + p_segmentIndex;
		Vector3 previousPosition = segment->m_vector0x00;

		LegoS32 nextIndex = p_segmentIndex + 1;
		if (nextIndex >= m_segmentCount) {
			nextIndex = m_closed != FALSE ? 0 : m_segmentCount - 1;
		}
		if (nextIndex == p_segmentIndex) {
			if (p_storeLength != 0) {
				segment->m_length = 0.0f;
				return totalLength;
			}
		}
		else {
			const Vector3& nextPosition = m_segments[nextIndex].m_vector0x00;
			if ((segment->m_flags & 1) != 0) {
				LegoFloat dx = (nextPosition.m_x - previousPosition.m_x) * p_t;
				LegoFloat dy = (nextPosition.m_y - previousPosition.m_y) * p_t;
				LegoFloat dz = (nextPosition.m_z - previousPosition.m_z) * p_t;
				totalLength = (LegoFloat) sqrt(dy * dy + dz * dz + dx * dx);
				if (p_storeLength != 0) {
					segment->m_length = totalLength;
					return totalLength;
				}
			}
			else {
				if ((g_splineCacheInitialized & 1) == 0) {
					g_splineCacheInitialized |= 1;
					g_splineSampleInterval = 0.0625f;
				}

				LegoS32 sampleIndex = 0;
				LegoBool32 cacheSample = TRUE;
				if (p_startT < p_t) {
					do {
						p_startT += g_splineSampleInterval;
						if (p_t < p_startT) {
							cacheSample = FALSE;
							p_startT = p_t;
						}

						LegoFloat sampleLength;
						if (m_cachedSampleIndex < sampleIndex || cacheSample == FALSE) {
							Vector3 position;
							FUN_004d0f60(&position, p_segmentIndex, p_startT);
							LegoFloat dx = position.m_x - previousPosition.m_x;
							LegoFloat dy = position.m_y - previousPosition.m_y;
							LegoFloat dz = position.m_z - previousPosition.m_z;
							previousPosition = position;
							sampleLength = (LegoFloat) sqrt(dy * dy + dz * dz + dx * dx);
							if (cacheSample != FALSE) {
								m_cachedPositions[sampleIndex] = position;
								m_cachedLengths[sampleIndex] = sampleLength;
								m_cachedSampleIndex = sampleIndex;
							}
						}
						else {
							previousPosition = m_cachedPositions[sampleIndex];
							sampleLength = m_cachedLengths[sampleIndex];
						}

						totalLength += sampleLength;
						++sampleIndex;
					} while (p_startT < p_t);
				}

				if (p_storeLength != 0) {
					segment->m_length = totalLength;
				}
			}
		}
	}

	return totalLength;
}

// FUNCTION length: 0x1da bytes (0x004d1d90-0x004d1f69).
// STUB: LEGORACERS2 0x004d1d90
LegoFloat SplinePath::FUN_004d1d90(Vector3* p_result, LegoFloat p_distance, LegoS32* p_segmentIndex)
{
	LegoFloat distance;
	if (p_distance <= m_totalLength) {
		distance = p_distance;
		if (p_distance < 0.0f && m_closed != FALSE) {
			distance = p_distance + m_totalLength;
		}
	}
	else if (m_closed == FALSE) {
		distance = m_totalLength;
	}
	else {
		distance = p_distance - m_totalLength;
	}

	LegoS32 segmentLimit = m_segmentCount;
	if (m_closed == FALSE) {
		--segmentLimit;
	}

	LegoS32 segmentIndex = 0;
	LegoFloat segmentStart = 0.0f;
	if (0.0f <= distance) {
		LegoFloat nextSegmentStart = 0.0f;
		do {
			segmentStart = nextSegmentStart;
			LegoFloat segmentLength = m_segments[segmentIndex].m_length;
			++segmentIndex;
			if (segmentIndex == segmentLimit) {
				break;
			}
			nextSegmentStart = segmentStart + segmentLength;
		} while (nextSegmentStart <= distance);
	}

	distance -= segmentStart;
	--segmentIndex;
	LegoFloat inverseSegmentLength = 1.0f / m_segments[segmentIndex].m_length;
	LegoFloat t = inverseSegmentLength * distance;
	LegoFloat previousT;
	LegoFloat previousError = 0.0f;
	LegoFloat lowerBound = distance - 0.01f;
	LegoFloat upperBound = distance + 0.01f;
	LegoFloat evaluatedLength = distance;
	LegoS32 iteration = 0;
	for (;;) {
		LegoFloat error = distance - evaluatedLength;
		LegoFloat nextT;
		if (iteration < 2) {
			nextT = error * inverseSegmentLength + t;
		}
		else if (error == previousError) {
			nextT = (previousT + t) * 0.5f;
		}
		else {
			LegoFloat ratio = error / previousError;
			nextT = (t - ratio * previousT) / (1.0f - ratio);
		}

		previousT = t;
		if (0.0f <= nextT) {
			t = nextT;
			if (1.0f < nextT) {
				t = 1.0f;
			}
		}
		else {
			t = 0.0f;
		}

		evaluatedLength = FUN_004d1aa0(segmentIndex, t, 0.0f, 0);
		if (!(upperBound < evaluatedLength || evaluatedLength < lowerBound)) {
			break;
		}

		++iteration;
		previousError = error;
		if (iteration >= 6) {
			break;
		}
	}

	FUN_004d0f60(p_result, segmentIndex, t);
	*p_segmentIndex = segmentIndex;
	return t;
}
