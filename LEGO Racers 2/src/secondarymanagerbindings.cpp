#include "gamecontroller.h"
#include "inputcontrollermanager.h"
#include "inputdeviceservice.h"
#include "secondarymanager.h"

// GLOBAL: LEGORACERS2 0x005305e0
const LegoU32 g_menuKeyboardBindingControls[11] = {0xcb, 0xcd, 0xc8, 0xd0, 0x1c, 0x01, 0x50, 0x48, 0x4b, 0x4d, 0x01};

// GLOBAL: LEGORACERS2 0x0053060c
const LegoU32 g_menuJoystickBindingControls[9] = {0, 4, 5, 2, 6, 7, 8, 1, 3};

// STUB: LEGORACERS2 0x0042af70
void SecondaryManagerMediaState::InitializeBindings()
{
	LegoU32 owner = (LegoU32) g_gameController.m_secondaryManager;
	if (g_inputService->m_vtable->findDevice(g_inputService, NULL, 0, 3) != NULL) {
		EnableSlot(0);
	}
	if (g_inputService->m_vtable->findDevice(g_inputService, NULL, 1, 3) != NULL) {
		EnableSlot(1);
	}
	if (g_inputService->m_vtable->findDevice(g_inputService, NULL, 0, 1) != NULL) {
		EnableSlot(2);
	}
	for (LegoU32 slot = 0; slot < 4; ++slot) {
		if (!IsSlotEnabled(slot)) {
			continue;
		}
		g_inputControllerManager->ConfigureController(owner, slot, slot == 3, FALSE);
		if (slot != 3) {
			g_inputControllerManager->FindOrCreateController(owner, slot, FALSE, TRUE)->m_liveInput = TRUE;
		}
		ControllerBinding binding;
		if (slot < 2) {
			binding.m_deviceType = 3;
			binding.m_deviceIndex = slot;
			for (LegoU32 control = 0; control < 11; ++control) {
				if (control < 2) {
					binding.m_unk0x00 = 0;
					binding.m_unk0x04 = control & 1;
					binding.m_controlKind = 0;
					binding.m_controlIndex = control & 1;
				}
				else {
					binding.m_unk0x00 = 3;
					binding.m_unk0x04 = control - 2;
					binding.m_controlKind = 6;
					binding.m_controlIndex = g_menuJoystickBindingControls[control - 2];
				}
				g_inputControllerManager->AppendControllerBinding(owner, slot, &binding);
			}
		}
		else if (slot == 2) {
			binding.m_deviceType = 1;
			binding.m_deviceIndex = 0;
			binding.m_controlKind = 6;
			for (LegoU32 control = 0; control < 11; ++control) {
				binding.m_unk0x00 = control < 4 ? (control & 1) + 1 : 3;
				binding.m_unk0x04 = control < 4 ? (control >> 1) & 1 : control - 4;
				binding.m_controlIndex = g_menuKeyboardBindingControls[control];
				g_inputControllerManager->AppendControllerBinding(owner, slot, &binding);
			}
			binding.m_unk0x00 = 3;
			binding.m_unk0x04 = 9;
			binding.m_controlIndex = 1;
			g_inputControllerManager->AppendControllerBinding(owner, slot, &binding);
		}
		else {
			binding.m_deviceType = 2;
			binding.m_deviceIndex = 0;
			for (LegoU32 control = 0; control < 14; ++control) {
				binding.m_unk0x00 = control < 2 ? 0 : 3;
				binding.m_unk0x04 = control < 2 ? control : control - 2;
				binding.m_controlKind = control < 2 ? 3 : 6;
				binding.m_controlIndex = control < 2 ? control : control - 2;
				g_inputControllerManager->AppendControllerBinding(owner, slot, &binding);
			}
		}
	}
}

// STUB: LEGORACERS2 0x0042b1e0
void SecondaryManagerMediaState::EnableSlot(LegoU32 p_index)
{
	m_enabledSlots |= 1u << (p_index & 31);
}
