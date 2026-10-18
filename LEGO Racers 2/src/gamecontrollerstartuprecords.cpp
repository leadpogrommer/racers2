#include "controllereventrecords.h"
#include "gamecontrollerinitialization.h"

namespace
{
struct ControllerSelectionState {
	LegoU32 m_count;
	LegoS32 m_index;
	LegoU32 m_unk0x08;
	LegoU32 m_unk0x0c;
	undefined m_unk0x10[0x50 - 0x10];
	LegoU32 m_slots[18];
};
DECOMP_STATIC_ASSERT(offsetof(ControllerSelectionState, m_slots) == 0x50);
DECOMP_SIZE_ASSERT(ControllerSelectionState, 0x98);

} // namespace

// FUNCTION: LEGORACERS2 0x004338c0
void* __fastcall ConstructControllerSelectionState(void* p_storage, void*)
{
	ControllerSelectionState* state = (ControllerSelectionState*) p_storage;
	state->m_count = 0;
	state->m_index = -1;
	state->m_unk0x08 = 0;
	state->m_unk0x0c = 0;
	for (LegoU32 i = 0; i < 18; ++i) {
		state->m_slots[i] = 0;
	}
	return state;
}

// STUB: LEGORACERS2 0x00434240
void* __fastcall ConstructControllerEventRecords(void* p_storage, void*)
{
	ControllerEventRecordTable* table = (ControllerEventRecordTable*) p_storage;
	table->m_flags = 0;
	LegoU32 i;
	for (i = 0; i < 280; ++i) {
		table->m_records[i].m_index = 0;
		// Retail calls its vector-construction helper twice per record with
		// the verified no-write initializer409190. The item bytes stay untouched.
	}
	for (i = 0; i < 280; ++i) {
		table->m_records[i].m_index = -1;
	}
	table->m_firstIndex = -1;
	table->m_secondIndex = -1;
	return table;
}
