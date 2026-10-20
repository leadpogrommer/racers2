#include "gamecontroller.h"
#include "inputcontroller.h"

namespace
{
LegoU32 GetControllerBindingControlKind(LegoU32 p_kind, LegoU32 p_alternate)
{
	if (p_kind == 1) {
		return (p_alternate != 0 ? 3 : 0) + 2;
	}
	if (p_kind == 2) {
		return (p_alternate != 0 ? 3 : 0) + 1;
	}
	return 6;
}
} // namespace

// STUB: LEGORACERS2 0x0040ad60
void __fastcall ConfigureControllerInputBindings(
	GameControllerPairGroups* p_profile,
	void*,
	InputController* p_controller
)
{
	p_controller->m_bindingCount = 0;
	ControllerBinding binding;
	binding.m_deviceType = p_profile->m_enabled;
	binding.m_deviceIndex = p_profile->m_enabled == 3 ? p_profile->m_index : 0;
	if (p_profile->m_enabled == 3) {
		binding.m_unk0x00 = 0;
		binding.m_unk0x04 = 0;
		binding.m_controlKind = p_profile->m_unk0x08 != 0 ? 3 : 0;
		binding.m_controlIndex = 0;
		p_controller->AppendBinding(&binding);
	}
	else {
		for (LegoU32 i = 0; i < 2; ++i) {
			binding.m_unk0x00 = (i & 1) + 1;
			binding.m_unk0x04 = (i >> 1) & 1;
			binding.m_controlKind =
				GetControllerBindingControlKind(p_profile->m_pairs[i].m_first, p_profile->m_unk0x08);
			binding.m_controlIndex = p_profile->m_pairs[i].m_second;
			p_controller->AppendBinding(&binding);
		}
	}
	const GameControllerPair* actions =
		p_profile->m_enabled == 1 ? p_profile->m_firstGroups : p_profile->m_secondGroups;
	binding.m_unk0x00 = 3;
	for (LegoU32 j = 0; j < 9; ++j) {
		binding.m_unk0x04 = j;
		binding.m_controlIndex = actions[j].m_second;
		binding.m_controlKind = GetControllerBindingControlKind(actions[j].m_first, p_profile->m_unk0x08);
		p_controller->AppendBinding(&binding);
	}
	LegoU32 finalControl = (LegoU32) p_controller;
	if (p_profile->m_enabled == 1) {
		finalControl = 1;
	}
	else if (p_profile->m_enabled == 3) {
		finalControl = 8;
	}
	binding.m_controlIndex = finalControl & 0xff;
	binding.m_unk0x04 = 9;
	binding.m_unk0x00 = 3;
	binding.m_controlKind = 6;
	p_controller->AppendBinding(&binding);
}
