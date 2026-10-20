#include "input/controller/inputcontrollermanager.h"
#include "input/service/inputdeviceservice.h"

// STUB: LEGORACERS2 0x004b8010
void InputControllerManager::RebindController(
	LegoU32 p_ownerKey,
	LegoU32 p_newOwnerKey,
	LegoU32 p_index,
	LegoU32 p_newIndex
)
{
	InputController* controller = FindOrCreateController(p_ownerKey, p_index, FALSE, TRUE);
	controller->SetOwner(p_newOwnerKey, p_newIndex, 1);
}

// STUB: LEGORACERS2 0x004b8040
void InputControllerManager::ConfigureController(
	LegoU32 p_ownerKey,
	LegoU32 p_index,
	LegoBool32 p_record,
	LegoBool32 p_defaults
)
{
	InputController* controller = FindOrCreateController(p_ownerKey, p_index, TRUE, p_record);
	controller->m_unk0x168 = 0;
	if (p_defaults == 0) {
		return;
	}
	ControllerBinding axes[2];
	ControllerBinding buttons[12];
	LegoU32 i;
	if (g_inputService->m_vtable->findDevice(g_inputService, NULL, p_index, 3) != NULL) {
		for (i = 0; i < 2; ++i) {
			axes[i].m_unk0x00 = 0;
			axes[i].m_unk0x04 = i & 1;
			axes[i].m_deviceType = 3;
			axes[i].m_deviceIndex = p_index;
			axes[i].m_controlKind = 0;
			axes[i].m_controlIndex = i & 1;
		}
		for (i = 0; i < 12; ++i) {
			buttons[i].m_unk0x00 = 3;
			buttons[i].m_unk0x04 = i;
			buttons[i].m_deviceType = 3;
			buttons[i].m_deviceIndex = p_index;
			buttons[i].m_controlKind = 6;
			buttons[i].m_controlIndex = i;
		}
	}
	else {
		const LegoU8 axisKeys[2] = {0x2c, 0x2d};
		const LegoU8 buttonKeys[12] = {0x27, 0x28, 0x34, 2, 3, 4, 0, 0, 0, 0, 0, 0};
		for (i = 0; i < 2; ++i) {
			axes[i].m_unk0x00 = (i & 1) + 1;
			axes[i].m_unk0x04 = 0;
			axes[i].m_deviceType = 1;
			axes[i].m_deviceIndex = 0;
			axes[i].m_controlKind = 6;
			axes[i].m_controlIndex = axisKeys[i];
		}
		for (i = 0; i < 12; ++i) {
			buttons[i].m_unk0x00 = 3;
			buttons[i].m_unk0x04 = i;
			buttons[i].m_deviceType = 1;
			buttons[i].m_deviceIndex = 0;
			buttons[i].m_controlKind = 6;
			buttons[i].m_controlIndex = buttonKeys[i];
		}
	}
	for (i = 0; i < 2; ++i) {
		AppendControllerBinding(p_ownerKey, p_index, &axes[i]);
	}
	for (i = 0; i < 12; ++i) {
		AppendControllerBinding(p_ownerKey, p_index, &buttons[i]);
	}
}

// STUB: LEGORACERS2 0x004b8220
void InputControllerManager::AppendControllerBinding(
	LegoU32 p_ownerKey,
	LegoU32 p_index,
	const ControllerBinding* p_binding
)
{
	InputController* controller = FindOrCreateController(p_ownerKey, p_index, FALSE, TRUE);
	controller->AppendBinding(p_binding);
}
