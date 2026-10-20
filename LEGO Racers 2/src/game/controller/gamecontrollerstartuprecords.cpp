#include "frontend/resources/controllereventrecords.h"
#include "frontend/resources/controllerloadingprogress.h"
#include "game/controller/gamecontrollerinitialization.h"

// FUNCTION: LEGORACERS2 0x004338c0
void* __fastcall ConstructControllerSelectionState(void* p_storage, void*)
{
	ControllerLoadingProgress* state = (ControllerLoadingProgress*) p_storage;
	state->m_active = 0;
	state->m_mode = -1;
	state->m_progress = 0.0f;
	state->m_elapsed = 0.0f;
	for (LegoU32 i = 0; i < 18; ++i) {
		state->m_sprites[i] = NULL;
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
