#ifndef LR2_FOYERSCENEACTIONSBASE_H
#define LR2_FOYERSCENEACTIONSBASE_H

#include "decomp.h"
#include "types.h"

struct FoyerSceneActionDispatch {
	void(__fastcall* m_update)(void*, void*);
	void(__fastcall* m_destroyState)(void*, void*);
	void(__fastcall* m_remove)(void*, void*);
	void(__fastcall* m_scale)(void*, void*, LegoFloat);
};

class SplinePath;
extern LegoU32 g_foyerSceneActionDefaults[23];
void __fastcall BindFoyerActionPath(void*, void*, SplinePath*);

void __cdecl InitializeFoyerSceneActionDefaultsAtStartup();
void __cdecl InitializeFoyerSceneActionDefaults();
void __fastcall InitializeFoyerSceneActionDefaultRecord(LegoU32* p_words);
void __fastcall SetFoyerSceneEventFaceState(void* p_event, void*, LegoU32 p_faceState, LegoFloat p_duration);
void __fastcall SetFoyerSceneEventPosition(void* p_event, void*, const LegoFloat* p_position);
void __fastcall SetFoyerSceneEventOrientation(void* p_event, void*, const LegoFloat* p_orientation);
void __fastcall ClearFoyerSceneEventAttachment(void* p_event, void*, const void* p_attachment);
void __fastcall AttachFoyerSceneEventTarget(void* p_event, void*, void* p_target, const LegoFloat* p_offset);
void __fastcall InvokeFoyerSceneInputAction(void* p_resource, void*, LegoU32 p_index);
void __fastcall InitializeFoyerSceneAction0(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction1(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction2(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction3(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction4(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction5(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction6(void* p_action, void*, const LegoU32* p_words);
void __fastcall InitializeFoyerSceneAction7(void* p_action, void*, const LegoU32* p_words);

#endif // LR2_FOYERSCENEACTIONSBASE_H
