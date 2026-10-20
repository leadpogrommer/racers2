#ifndef LR2_INPUTCONTROLLER_H
#define LR2_INPUTCONTROLLER_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct ControllerBinding {
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;
	LegoU32 m_deviceType;
	LegoU32 m_deviceIndex;
	LegoU32 m_controlKind;
	LegoU32 m_controlIndex;
};
DECOMP_SIZE_ASSERT(ControllerBinding, 0x18);

class InputController {
public:
	InputController(LegoBool32 p_recordInput);
	~InputController();
	void SetOwner(LegoU32 p_ownerKey, LegoU32 p_index, LegoU32 p_unused);
	void AppendBinding(const ControllerBinding* p_binding);
	ControllerBinding* GetBinding(LegoS32 p_index);
	LegoU32* GetRecordedInput(LegoU32 p_frame);
	LegoU32* GetCurrentInput();
	void RecordInputRange(const LegoU32* p_input, LegoU32 p_firstFrame, LegoU32 p_lastFrame);

	LegoU32 m_state;       // 0x00
	LegoU32 m_ownerKey;    // 0x04; game-object identity, not a device kind
	LegoU32 m_index;       // 0x08
	void* m_recordedInput; // 0x0c
	LegoU8 m_unk0x10;
	LegoU8 m_unk0x11;
	LegoU16 m_unk0x12;
	LegoS32 m_bindingCount;           // 0x14
	ControllerBinding m_bindings[14]; // 0x18
	LegoU32 m_unk0x168;
	LegoS32 m_recordedFrame;  // 0x16c
	LegoBool32 m_recordInput; // 0x170
	LegoBool32 m_liveInput;   // 0x174
	LegoBool32 m_disabled;    // 0x178
};

DECOMP_SIZE_ASSERT(InputController, 0x17c);
DECOMP_STATIC_ASSERT(offsetof(InputController, m_bindings) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(InputController, m_recordedFrame) == 0x16c);

#endif // LR2_INPUTCONTROLLER_H
