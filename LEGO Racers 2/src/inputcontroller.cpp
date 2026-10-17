#include "inputcontroller.h"

#include "memoryheap.h"

#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x00679d2c
static LegoU32 g_missingRecordedControllerInput;
// GLOBAL: LEGORACERS2 0x00679d30
static LegoU8 g_missingRecordedControllerInputInitialized;
// GLOBAL: LEGORACERS2 0x00679d31
static LegoU8 g_emptyCurrentControllerInputInitialized;
// GLOBAL: LEGORACERS2 0x00679d40
static LegoU32 g_emptyCurrentControllerInput;

static void DestroyMissingRecordedControllerInput()
{
}

static void DestroyEmptyCurrentControllerInput()
{
}

// STUB: LEGORACERS2 0x004b7b80
InputController::InputController(LegoBool32 p_recordInput)
{
	m_state = 1;
	m_deviceType = 0;
	m_deviceIndex = 0;
	m_recordedInput = NULL;
	m_unk0x12 = 0;
	m_unk0x10 = 0;
	m_unk0x11 = 0;
	m_bindingCount = 0;
	for (LegoU32 index = 0; index < sizeOfArray(m_bindings); index++) {
		ControllerBinding& binding = m_bindings[index];
		binding.m_unk0x00 = 3;
		binding.m_unk0x04 = 0;
		binding.m_deviceType = 1;
		binding.m_deviceIndex = 0;
		binding.m_controlKind = 6;
		binding.m_controlIndex = 0;
	}
	m_unk0x168 = 0;
	m_recordedFrame = -1;
	m_recordInput = p_recordInput;
	m_liveInput = p_recordInput == 0;
	m_disabled = 0;
	if (p_recordInput != 0) {
		m_recordedInput = GetMemoryHeap()->Allocate(0x10000, "Controller Data");
	}
}

// FUNCTION: LEGORACERS2 0x004b7c20
InputController::~InputController()
{
	if (m_recordedInput != NULL) {
		MemoryHeap* heap = GetMemoryHeap();
		heap->Free(m_recordedInput);
	}
	m_recordedInput = NULL;
}

// FUNCTION: LEGORACERS2 0x004b7d80
void InputController::SetDevice(LegoU32 p_type, LegoU32 p_index, LegoU32 p_unused)
{
	m_deviceType = p_type;
	m_deviceIndex = p_index;
}

// FUNCTION: LEGORACERS2 0x004b7da0
void InputController::AppendBinding(const ControllerBinding* p_binding)
{
	m_bindings[m_bindingCount] = *p_binding;
	m_bindingCount++;
}

// FUNCTION: LEGORACERS2 0x004b7e30
ControllerBinding* InputController::GetBinding(LegoS32 p_index)
{
	return &m_bindings[p_index];
}

// STUB: LEGORACERS2 0x004b7e40
LegoU32* InputController::GetRecordedInput(LegoU32 p_frame)
{
	if ((LegoU32) m_recordedFrame < p_frame) {
		if ((g_missingRecordedControllerInputInitialized & 1) == 0) {
			g_missingRecordedControllerInputInitialized |= 1;
			g_missingRecordedControllerInput = 0;
			atexit(DestroyMissingRecordedControllerInput);
		}
		return &g_missingRecordedControllerInput;
	}
	return (LegoU32*) m_recordedInput + (p_frame & 0x3fff);
}

// STUB: LEGORACERS2 0x004b7eb0
LegoU32* InputController::GetCurrentInput()
{
	if (m_recordedInput == NULL) {
		return (LegoU32*) &m_unk0x10;
	}
	if ((g_emptyCurrentControllerInputInitialized & 1) == 0) {
		g_emptyCurrentControllerInputInitialized |= 1;
		g_emptyCurrentControllerInput = 0;
		atexit(DestroyEmptyCurrentControllerInput);
	}
	if (m_recordedFrame == -1) {
		return &g_emptyCurrentControllerInput;
	}
	return GetRecordedInput(m_recordedFrame);
}

// STUB: LEGORACERS2 0x004b7f20
void InputController::RecordInputRange(const LegoU32* p_input, LegoU32 p_firstFrame, LegoU32 p_lastFrame)
{
	for (; p_firstFrame <= p_lastFrame; p_firstFrame++) {
		((LegoU32*) m_recordedInput)[p_firstFrame & 0x3fff] = *p_input;
	}
	if (p_lastFrame > 0x3fff) {
		m_state = 0;
	}
}
