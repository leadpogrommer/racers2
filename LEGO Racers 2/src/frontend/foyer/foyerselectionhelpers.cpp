#include "frontend/foyer/foyerselectionhelpers.h"

#include "frontend/overlay/textboxmanager.h"

// STUB: LEGORACERS2 0x0043c330
LegoS32 __fastcall GetFoyerObjectSelection(void* p_objectState, void*, const LegoFloat* p_statePosition)
{
	LegoFloat x = p_statePosition[0] - *(LegoFloat*) ((LegoU8*) p_objectState + 0x58);
	LegoFloat y = p_statePosition[1] - *(LegoFloat*) ((LegoU8*) p_objectState + 0x5c);
	LegoFloat z = p_statePosition[2] - *(LegoFloat*) ((LegoU8*) p_objectState + 0x60);
	LegoFloat radiusSquared = *(LegoFloat*) ((LegoU8*) p_objectState + 0x64);
	return x * x + y * y + z * z < radiusSquared;
}

// STUB: LEGORACERS2 0x00460070
void __fastcall ClearFoyerNpcText(GameObject* p_textState, void*)
{
	LegoU8* state = (LegoU8*) p_textState;
	if (*(LegoS32*) (state + 0x5c) != -1) {
		GameObject* textObject = *(GameObject**) (state + 0x60);
		if (textObject != NULL && *(LegoU32*) ((LegoU8*) textObject + 0x58) != 0) {
			ClearTextBoxManagerResources(textObject, NULL);
			*(GameObject**) (state + 0x60) = NULL;
		}
		*(LegoS32*) (state + 0x5c) = -1;
	}
}

// STUB: LEGORACERS2 0x004801c0
LegoU32 __fastcall IsFoyerSceneTransitionComplete(FoyerSceneResourceView* p_resource, void*)
{
	LegoS32 state = *(LegoS32*) ((LegoU8*) p_resource + 0x58);
	return state >= 1 && state <= 4;
}
