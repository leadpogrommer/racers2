#include "controllerrecordingservice.h"
#include "inputcontrollermanager.h"
#include "systemclock.h"

#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x00679d34
static LegoU32 g_defaultControllerInput;
// GLOBAL: LEGORACERS2 0x00679d3c
static LegoU8 g_controllerInputGlobalsInitialized;

static void DestroyControllerInputGlobals()
{
}

// STUB: LEGORACERS2 0x004b8250
void InputControllerManager::GetControllerInputForOwner(
	LegoU32 p_ownerKey,
	LegoU32* p_output,
	LegoU32 p_controllerIndex,
	LegoU32 p_forceLiveInput
)
{
	if ((g_controllerInputGlobalsInitialized & 1) == 0) {
		g_controllerInputGlobalsInitialized |= 1;
		g_defaultControllerInput = 0;
		atexit(DestroyControllerInputGlobals);
	}

	LegoU32 frame = m_unk0x18 / m_pollMultiplier;
	InputController* controller = FindOrCreateController(p_ownerKey, p_controllerIndex, FALSE, TRUE);
	*p_output = g_defaultControllerInput;
	if (controller == NULL) {
		goto normalize;
	}

	if (controller->m_disabled != 0 && frame > m_unk0x1c &&
		g_controllerRecordingService->m_vtable->m_isAvailable(g_controllerRecordingService, NULL) != FALSE) {
		LegoU32 serviceFrame = frame - m_unk0x1c;
		if (g_controllerRecordingService->m_vtable
				->m_hasInputFrame(g_controllerRecordingService, NULL, controller, serviceFrame) == FALSE) {
			g_controllerRecordingService->m_vtable
				->m_requestInputFrame(g_controllerRecordingService, NULL, controller, serviceFrame);
		}
	}

	if (m_paused == 1 || controller->m_liveInput == 1 || p_forceLiveInput == 1) {
		PollInput(TRUE);
		*p_output = *(const LegoU32*) ((const LegoU8*) controller + 0x10);
		goto normalize;
	}

	if (controller->m_disabled == 0) {
		while ((LegoU32) m_currentFrame < frame || m_currentFrame == -1) {
			if (controller->m_recordInput != 1) {
				return;
			}
			PollInput(FALSE);
		}
	}

	if (controller->m_recordInput != 1) {
		*p_output = *controller->GetRecordedInput(frame);
		goto normalize;
	}

	if (g_controllerRecordingService->m_vtable->m_isAvailable(g_controllerRecordingService, NULL) != FALSE &&
		controller->m_disabled == 0) {
		LegoU32 recordedValue = *controller->GetRecordedInput(frame);
		g_controllerRecordingService->m_vtable
			->m_recordInput(g_controllerRecordingService, NULL, controller, &recordedValue, frame);
	}

	if (m_unk0x1c <= frame) {
		*p_output = *controller->GetRecordedInput(frame - m_unk0x1c);
	}

normalize:
	if (m_unk0x68 == 1) {
		LegoS8 axis = (LegoS8) ((LegoU8*) p_output)[2];
		LegoFloat value = (LegoFloat) axis * -0.007874015718698502f;
		if (value < -1.0f) {
			value = -1.0f;
		}
		else if (value > 1.0f) {
			value = 1.0f;
		}
		((LegoU8*) p_output)[2] = (LegoU8) (LegoS8) (value * 127.0f);
	}
}

// STUB: LEGORACERS2 0x004b9040
void InputControllerManager::GetInputRequestElapsedTime(LegoU32* p_seconds, LegoU32* p_microseconds)
{
	SystemClock* clock = GetSystemClock();
	LegoU32* seconds = &m_unk0x98[7];
	LegoU32* microseconds = &m_unk0x98[8];
	clock->m_vtable->m_queryTime(clock, NULL, (LegoS32*) seconds, (LegoS32*) microseconds);

	*microseconds -= m_unk0x98[2];
	while ((LegoS32) *microseconds < 0) {
		--*seconds;
		*microseconds += 1000000;
	}
	*seconds -= m_unk0x98[1];
	*p_seconds = *seconds;
	*p_microseconds = *microseconds;
}
