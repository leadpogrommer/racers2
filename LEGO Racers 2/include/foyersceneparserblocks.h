#ifndef LR2_FOYERSCENEPARSERBLOCKS_H
#define LR2_FOYERSCENEPARSERBLOCKS_H

#include "foyerscene.h"

void __fastcall ParseFoyerSceneGlobalBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSoundBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSubtitleBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneActorBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneCinematicBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneYesNoBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSavePositionsBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneEffectBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneMobileBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneJibberBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneRenderSleepBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSparkyMenuBlock(FoyerSceneResource*, void*);

#endif // LR2_FOYERSCENEPARSERBLOCKS_H
