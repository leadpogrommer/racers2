#ifndef LR2_FOYERSCENEPARSER_H
#define LR2_FOYERSCENEPARSER_H

#include "foyerscene.h"

struct GameObject;

extern LegoChar g_foyerSceneParserToken[24];
extern LegoChar* g_foyerSceneParserBuffer;
extern LegoChar* g_foyerSceneParserCursor;
extern LegoChar* g_foyerSceneParserEnd;
extern LegoFloat g_foyerSceneParserTime;

LegoU32 __fastcall GetFoyerSceneTokenId(FoyerSceneResource* p_resource, void* p_unusedEdx, LegoU32 p_table);
LegoBool32 __fastcall IsFoyerSceneParserAtEnd(FoyerSceneResource* p_resource, void* p_unusedEdx);
GameObject* __fastcall ParseFoyerSceneObjectReference(
	FoyerSceneResource* p_resource,
	void* p_unusedEdx,
	LegoU32 p_mode
);
LegoFloat __fastcall ParseFoyerSceneNumber(FoyerSceneResource* p_resource, void* p_unusedEdx);
LegoS32 __fastcall ParseFoyerSceneInteger(FoyerSceneResource* p_resource, void* p_unusedEdx);
LegoU32 __fastcall ParseFoyerSceneAssetType(FoyerSceneResource* p_resource, void* p_unusedEdx, LegoU32 p_table);
void __fastcall ParseFoyerSceneVector3(FoyerSceneResource* p_resource, void* p_unusedEdx, LegoFloat* p_vector);
LegoS32 __fastcall FindFoyerSceneEventIndex(FoyerSceneResource* p_resource, void* p_unusedEdx);
void __fastcall AppendFoyerSceneAction(FoyerSceneResource* p_resource, void* p_unusedEdx, void* p_action);
LegoFloat __fastcall FindNextFoyerActionTime(
	FoyerSceneResource* p_resource,
	void* p_unusedEdx,
	LegoFloat p_afterTime,
	LegoU32 p_type
);

#endif // LR2_FOYERSCENEPARSER_H
