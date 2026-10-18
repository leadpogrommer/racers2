#include "inputcontrollermanager.h"
#include "inputdeviceservice.h"

#include <math.h>

namespace
{

void SetMappedButton(InputController* p_controller, LegoS32 p_index, LegoBool32 p_pressed)
{
	LegoU8* buttons;
	LegoU32 shift;
	if (p_index > 7) {
		buttons = &p_controller->m_unk0x10;
		shift = (p_index - 8) & 31;
	}
	else {
		buttons = &p_controller->m_unk0x11;
		shift = p_index & 31;
	}
	LegoU8 bit = (LegoU8) (1u << shift);
	if (p_pressed != FALSE) {
		*buttons |= bit;
	}
	else {
		*buttons &= ~bit;
	}
}

void SetMappedAxis(InputController* p_controller, LegoS32 p_index, LegoFloat p_value)
{
	// Retail clamps unordered comparisons at the lower limit, then uses FTOL
	// (truncation) before storing the low byte of the scaled signed value.
	if (!(p_value >= -1.0f)) {
		p_value = -1.0f;
	}
	else if (p_value > 1.0f) {
		p_value = 1.0f;
	}
	((LegoU8*) &p_controller->m_unk0x12)[p_index] = (LegoU8) (LegoS32) (p_value * 127.0f);
}

void ApplyDigitalAxis(
	LegoFloat* p_axes,
	LegoU32* p_flags,
	LegoS32 p_index,
	LegoS32 p_direction,
	LegoBool32 p_direct,
	LegoFloat p_step
)
{
	p_flags[p_index] |= 2;
	if (p_direct != FALSE) {
		p_axes[p_index] = (LegoFloat) p_direction;
	}
	else if (p_direction < 0) {
		LegoFloat value = p_axes[p_index] - p_step;
		p_axes[p_index] = !(value > -1.0f) ? -1.0f : value;
	}
	else {
		LegoFloat value = p_axes[p_index] + p_step;
		p_axes[p_index] = value >= 1.0f ? 1.0f : value;
	}
}

InputDeviceRecord* ResolveBindingDevice(
	InputControllerManager* p_manager,
	InputController* p_controller,
	const ControllerBinding* p_binding
)
{
	if (p_controller->m_unk0x168 != 0) {
		return (InputDeviceRecord*) p_controller->m_unk0x168;
	}
	switch (p_binding->m_deviceType) {
	case 1:
		return p_manager->m_keyboard;
	case 2:
		return p_manager->m_mouse;
	case 3:
		return g_inputService->m_vtable->findDevice(g_inputService, NULL, p_binding->m_deviceIndex, 3);
	}
	return NULL;
}

} // namespace

// STUB: LEGORACERS2 0x004b86f0
LegoBool32 InputControllerManager::PollController(InputController* p_controller, LegoU32 p_frames)
{
	LegoU32 previousInput = *p_controller->GetCurrentInput();
	LegoFloat frames = (LegoFloat) (LegoS32) p_frames;
	LegoFloat step = frames * m_axisScale0;
	LegoFloat decay = frames * m_axisScale1;
	if (p_controller->m_disabled != 0) {
		return FALSE;
	}
	LegoU32 axisFlags[2] = {0, 0};
	LegoFloat axes[2];
	LegoU32 axis;
	for (axis = 0; axis < 2; axis++) {
		axes[axis] = (LegoFloat) (LegoS8) (previousInput >> (16 + axis * 8)) * 0.007874015718698502f;
	}
	LegoBool32 direct = p_controller->m_liveInput != 0 || g_inputControllerManager->m_paused != 0;
	for (LegoU32 index = 0; index < (LegoU32) p_controller->m_bindingCount; index++) {
		ControllerBinding* binding = p_controller->GetBinding(index);
		InputDeviceRecord* device = ResolveBindingDevice(this, p_controller, binding);
		if (device == NULL || binding->m_controlKind > 7) {
			continue;
		}
		LegoU32 controlKind = binding->m_controlKind;
		LegoFloat value;
		if (controlKind < 6) {
			value = device->m_vtable->getAxisValue(device, NULL, binding->m_controlIndex);
		}
		else {
			value = device->m_vtable->getButtonValue(device, NULL, binding->m_controlIndex);
		}

		LegoS32 output = (LegoS32) binding->m_unk0x04;
		LegoU32 outputKind = binding->m_unk0x00;
		switch (controlKind) {
		case 0:
			if (outputKind == 0) {
				axisFlags[output] |= 1;
				if (value >= 0.5f) {
					ApplyDigitalAxis(axes, axisFlags, output, 1, direct, step);
				}
				else if (!(value > -0.5f)) {
					ApplyDigitalAxis(axes, axisFlags, output, -1, direct, step);
				}
			}
			else if (outputKind == 3 || outputKind == 4) {
				SetMappedButton(p_controller, output, (LegoFloat) fabs((double) value) >= 0.5f);
			}
			break;
		case 1:
			if (outputKind == 0) {
				axisFlags[output] |= 1;
				if (!(value > -0.5f)) {
					ApplyDigitalAxis(axes, axisFlags, output, -1, direct, step);
				}
			}
			else if ((outputKind == 3 || outputKind == 4) && !(value > -0.5f)) {
				SetMappedButton(p_controller, output, -value >= 0.5f);
			}
			break;
		case 2:
			if (outputKind == 0) {
				axisFlags[output] |= 1;
				if (value >= 0.5f) {
					ApplyDigitalAxis(axes, axisFlags, output, 1, direct, step);
				}
			}
			else if ((outputKind == 3 || outputKind == 4) && value >= 0.5f) {
				SetMappedButton(p_controller, output, TRUE);
			}
			break;
		case 3:
		case 4:
		case 5:
			if ((controlKind == 4 && value >= 0.0f) || (controlKind == 5 && !(value >= 0.0f))) {
				break;
			}
			if (outputKind == 0) {
				SetMappedAxis(p_controller, output, value);
			}
			else if (outputKind == 3 || outputKind == 4) {
				LegoFloat pressedValue = value;
				if (controlKind == 3) {
					pressedValue = value * 0.5f + 1.0f;
				}
				else if (controlKind == 4) {
					pressedValue = -value;
				}
				SetMappedButton(p_controller, output, pressedValue >= 0.5f);
			}
			break;
		case 6:
			if (outputKind == 1 || outputKind == 2) {
				axisFlags[output] |= 1;
				if (value >= 0.5f) {
					ApplyDigitalAxis(axes, axisFlags, output, outputKind == 1 ? -1 : 1, direct, step);
				}
			}
			else if (outputKind == 3 || outputKind == 4) {
				SetMappedButton(p_controller, output, value >= 0.5f);
			}
			break;
		case 7:
			if (outputKind == 1 || outputKind == 2) {
				SetMappedAxis(p_controller, output, outputKind == 1 ? -value : value);
			}
			else if (outputKind == 3 || outputKind == 4) {
				SetMappedButton(p_controller, output, value >= 0.5f);
			}
			break;
		}
	}

	for (axis = 0; axis < 2; axis++) {
		if (axisFlags[axis] == 1) {
			if (direct != FALSE) {
				axes[axis] = 0.0f;
			}
			else if (m_unk0x28 == 1) {
				if (!(axes[axis] >= 0.0f)) {
					LegoFloat value = axes[axis] + decay;
					axes[axis] = value >= 0.0f ? 0.0f : value;
				}
				else if (axes[axis] > 0.0f) {
					LegoFloat value = axes[axis] - decay;
					axes[axis] = value <= 0.0f ? 0.0f : value;
				}
			}
			else if (m_unk0x28 == 2) {
				axes[axis] *= 0.5f;
			}
		}
		if (axisFlags[axis] != 0) {
			SetMappedAxis(p_controller, axis, axes[axis]);
		}
	}
	if (p_controller->m_recordInput != 0) {
		p_controller->m_recordedFrame = m_currentFrame;
	}
	return TRUE;
}
