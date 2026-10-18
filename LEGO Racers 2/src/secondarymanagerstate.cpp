#include "gamecontroller.h"
#include "inputcontrollermanager.h"
#include "secondarymanager.h"

void DecodeSecondaryManagerMediaSlot(SecondaryManagerMediaSlot* p_slot, const LegoU8* p_state);

// STUB: LEGORACERS2 0x0042b200
LegoBool32 SecondaryManagerMediaState::IsSlotEnabled(LegoU32 p_index)
{
	return (m_enabledSlots & (1u << (p_index & 0x1f))) != 0;
}

// STUB: LEGORACERS2 0x0042b220
void SecondaryManagerMediaState::Refresh(LegoU32 p_mask)
{
	LegoU32 ownerKey = (LegoU32) g_gameController.m_secondaryManager;
	for (LegoU32 index = 0; index < 4; ++index) {
		if ((p_mask & (1u << (index & 0x1f))) != 0 && IsSlotEnabled(index) != FALSE) {
			LegoU32 input = 0;
			g_inputControllerManager->GetControllerInputForOwner(ownerKey, &input, index, index != 3);
			DecodeSecondaryManagerMediaSlot(GetSlot(index), (const LegoU8*) &input);
			if (index == 3) {
				m_currentSlot = input;
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x0042b2b0
SecondaryManagerMediaSlot* SecondaryManagerMediaState::GetSlot(LegoU32 p_index)
{
	return (SecondaryManagerMediaSlot*) ((LegoU8*) this + 4 + p_index * sizeof(SecondaryManagerMediaSlot));
}

// STUB: LEGORACERS2 0x0042b2c0
LegoBool32 SecondaryManagerMediaState::HasActiveSlot()
{
	LegoU32 index;
	for (index = 0; index < 4; ++index) {
		SecondaryManagerMediaSlot* slot = GetSlot(index);
		if (IsSlotEnabled(index) && (slot->m_state != 0 || slot->m_resourceId != 0)) {
			return TRUE;
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x004b92b0
void DecodeSecondaryManagerMediaSlot(SecondaryManagerMediaSlot* p_slot, const LegoU8* p_state)
{
	LegoU32 previousId = p_slot->m_resourceId;
	LegoU32 previousState = p_slot->m_state;
	LegoU32 nextId = ((LegoU32) p_state[0] << 8) | p_state[1];
	LegoFloat x = (LegoS8) p_state[2] * 0.007874015718698502f;
	LegoFloat y = (LegoS8) p_state[3] * 0.007874015718698502f;
	LegoU32 nextState = 0;

	if (x < -0.5f) {
		nextState |= 1;
	}
	else if (x > 0.5f) {
		nextState |= 2;
	}
	if (y < -0.5f) {
		nextState |= 4;
	}
	else if (y > 0.5f) {
		nextState |= 8;
	}

	p_slot->m_resourceId = nextId;
	p_slot->m_resourceChangeMask = ~previousId & nextId;
	p_slot->m_state = nextState;
	p_slot->m_stateChangeMask = ~previousState & nextState;
}
