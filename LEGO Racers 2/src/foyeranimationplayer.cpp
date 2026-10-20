#include "foyeranimationmaster.h"
#include "memoryheap.h"

struct FoyerAnimationFramePrefix {
	Vector3 m_position;
	Vector3 m_velocity;
};
DECOMP_SIZE_ASSERT(FoyerAnimationFramePrefix, 0x18);

static const FoyerAnimationFramePrefix* GetPlayerAnimationFrame(
	const FoyerAnimationMasterEntry* p_track,
	LegoS32 p_frame
)
{
	return (const FoyerAnimationFramePrefix*) (p_track->m_body + p_track->m_frameStride * p_frame);
}

static LegoS32 GetPlayerAnimationFrameIndex(LegoFloat p_phase)
{
	LegoS32 frame;
	LegoU16 status;
	// Retail rounds with the current x87 control word and then corrects the
	// rounded integer downward when it lies above the phase.
	// clang-format off
	__asm {
		fld p_phase
		fistp frame
		fild frame
		fcomp p_phase
		fnstsw status
	}
	// clang-format on
	if ((status & 0x4100) == 0)
	{
		--frame;
	}
	return frame;
}

static LegoBool32 PlayerAnimationEqualOrUnordered(LegoFloat p_first, LegoFloat p_second)
{
	LegoU16 status;
	// VC6 lowers a C++ less-than branch using C0 alone, accepting unordered
	// operands. Inspect C3 directly for the retail equal-or-unordered branch.
	// clang-format off
	__asm {
		fld p_first
		fcomp p_second
		fnstsw status
	}
	// clang-format on
	return (status & 0x4000) != 0;
}

static LegoBool32 CalculatePlayerAnimationAdvance(
	LegoFloat p_frameRate,
	LegoFloat p_rate,
	LegoFloat p_step,
	LegoFloat* p_advance
)
{
	const LegoFloat zero = 0.0f;
	LegoU16 status;
	// Keep the computed advance in x87 precision for the comparison, while
	// storing the float used by the subsequent phase and velocity operations.
	// clang-format off
	__asm {
		mov eax, p_advance
		fld p_frameRate
		fmul p_rate
		fmul p_step
		fst dword ptr [eax]
		fcomp zero
		fnstsw status
	}
	// clang-format on
	return (status & 0x4000) == 0;
}

// STUB: LEGORACERS2 0x004f6840
FoyerAnimationPlayer::FoyerAnimationPlayer(LegoU32 p_skeletonCount)
{
	m_skeletonCount = p_skeletonCount;
	m_state = 0;
	m_track = NULL;
	m_rate = 0.0f;
	m_pose.m_phase = 0.0f;
	m_pose.m_velocity.m_x = 0.0f;
	m_pose.m_velocity.m_y = 0.0f;
	m_pose.m_velocity.m_z = 0.0f;
	m_pose.m_quaternions =
		(Quaternion*) GetMemoryHeap()->Allocate(p_skeletonCount << 4, "Frame Quat space for animplayer");
}

// STUB: LEGORACERS2 0x004f68a0
FoyerAnimationPlayer::~FoyerAnimationPlayer()
{
	GetMemoryHeap()->Free(m_pose.m_quaternions);
}

// FUNCTION: LEGORACERS2 0x004f68c0
void FoyerAnimationPlayer::Bind(const FoyerAnimationMasterEntry* p_track, LegoFloat p_rate, LegoFloat p_weight)
{
	m_track = p_track;
	m_rate = p_rate;
	m_weight = p_weight;
	m_pose.m_phase = 0.0f;
	m_state = 1;
}

// STUB: LEGORACERS2 0x004f68f0
void FoyerAnimationPlayer::Update(LegoFloat p_step, LegoU32 p_updateQuaternions)
{
	LegoFloat frameCount = m_track->m_frameCountFloat;
	LegoFloat advance;
	if (!CalculatePlayerAnimationAdvance(m_track->m_frameRate, m_rate, p_step, &advance)) {
		return;
	}
	LegoFloat phase = m_pose.m_phase + advance;
	// The original negative-wrap branch also accepts an unordered comparison.
	while (!(phase >= 0.0f)) {
		phase += frameCount;
	}
	while (phase >= frameCount) {
		phase -= frameCount;
	}
	m_pose.m_phase = phase;
	if (PlayerAnimationEqualOrUnordered(m_weight, 0.0f)) {
		return;
	}

	LegoS32 frameIndex = GetPlayerAnimationFrameIndex(phase);
	LegoS32 nextFrameIndex = frameIndex + 1;
	LegoFloat firstWeight = 1.0f - (phase - (LegoFloat) frameIndex);
	if (nextFrameIndex >= m_track->m_frameCount) {
		nextFrameIndex = 0;
	}
	const FoyerAnimationFramePrefix* firstFrame = GetPlayerAnimationFrame(m_track, frameIndex);
	if (nextFrameIndex == frameIndex) {
		if ((m_track->m_flags & 1) == 0) {
			m_pose.m_position = firstFrame->m_position;
			m_pose.m_velocity.m_x = 0.0f;
			m_pose.m_velocity.m_y = 0.0f;
			m_pose.m_velocity.m_z = 0.0f;
		}
		else {
			m_pose.m_position.m_x = 0.0f;
			m_pose.m_position.m_y = 0.0f;
			m_pose.m_position.m_z = 0.0f;
			m_pose.m_velocity = firstFrame->m_velocity;
			m_pose.m_velocity.m_x *= m_rate;
			m_pose.m_velocity.m_y *= m_rate;
			m_pose.m_velocity.m_z *= m_rate;
		}
		if (p_updateQuaternions != 0) {
			const Quaternion* source = (const Quaternion*) (firstFrame + 1);
			for (LegoS32 i = 0; i < (LegoS32) m_skeletonCount; ++i) {
				m_pose.m_quaternions[i] = source[i];
			}
		}
		return;
	}

	const FoyerAnimationFramePrefix* nextFrame = GetPlayerAnimationFrame(m_track, nextFrameIndex);
	if ((m_track->m_flags & 1) == 0) {
		LegoFloat firstY = firstWeight * firstFrame->m_position.m_y;
		LegoFloat firstZ = firstWeight * firstFrame->m_position.m_z;
		LegoFloat nextWeight = 1.0f - firstWeight;
		m_pose.m_position.m_x = nextWeight * nextFrame->m_position.m_x + firstWeight * firstFrame->m_position.m_x;
		m_pose.m_position.m_y = nextWeight * nextFrame->m_position.m_y + firstY;
		m_pose.m_position.m_z = nextWeight * nextFrame->m_position.m_z + firstZ;
		m_pose.m_velocity.m_x = 0.0f;
		m_pose.m_velocity.m_y = 0.0f;
		m_pose.m_velocity.m_z = 0.0f;
	}
	else {
		// Integration starts at the phase already advanced above. This second
		// interval is the retail sequence, including its handling of reverse rates.
		LegoFloat endPhase = phase + advance;
		m_pose.m_position.m_x = 0.0f;
		m_pose.m_position.m_y = 0.0f;
		m_pose.m_position.m_z = 0.0f;
		Vector3 integrated = {0.0f, 0.0f, 0.0f};
		while (phase < endPhase) {
			LegoFloat segment = endPhase - phase;
			if (segment > 1.0f) {
				segment = 1.0f;
			}
			LegoS32 integrationFrame = GetPlayerAnimationFrameIndex(phase);
			LegoFloat untilNextFrame = ((LegoFloat) integrationFrame + 1.0f) - phase;
			if (segment > untilNextFrame) {
				segment = untilNextFrame;
			}
			const Vector3* velocity = &GetPlayerAnimationFrame(m_track, integrationFrame)->m_velocity;
			integrated.m_x += segment * velocity->m_x;
			integrated.m_y += segment * velocity->m_y;
			integrated.m_z += segment * velocity->m_z;
			phase += segment;
			if (phase >= frameCount) {
				phase -= frameCount;
				endPhase -= frameCount;
			}
		}
		LegoFloat velocityScale = m_rate / advance;
		m_pose.m_velocity.m_x = velocityScale * integrated.m_x;
		m_pose.m_velocity.m_y = integrated.m_y * velocityScale;
		m_pose.m_velocity.m_z = integrated.m_z * velocityScale;
	}

	if (p_updateQuaternions == 0) {
		return;
	}
	const Quaternion* firstQuaternions = (const Quaternion*) (firstFrame + 1);
	const Quaternion* nextQuaternions = (const Quaternion*) (nextFrame + 1);
	// These comparisons mirror the x87 equality bit, which is also set for
	// unordered values in the two direct-copy branches.
	if (PlayerAnimationEqualOrUnordered(firstWeight, 1.0f)) {
		for (LegoS32 i = 0; i < (LegoS32) m_skeletonCount; ++i) {
			m_pose.m_quaternions[i] = firstQuaternions[i];
		}
	}
	else if (PlayerAnimationEqualOrUnordered(firstWeight, 0.0f)) {
		for (LegoS32 j = 0; j < (LegoS32) m_skeletonCount; ++j) {
			m_pose.m_quaternions[j] = nextQuaternions[j];
		}
	}
	else {
		for (LegoS32 k = 0; k < (LegoS32) m_skeletonCount; ++k) {
			m_pose.m_quaternions[k].Interpolate(&firstQuaternions[k], &nextQuaternions[k], firstWeight);
		}
	}
}
