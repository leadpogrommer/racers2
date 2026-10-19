#ifndef LR2_MODELRESOURCECHUNKS_H
#define LR2_MODELRESOURCECHUNKS_H

#include "modelresource.h"

class WinFile;

void* __cdecl LoadCollisionResource(void* p_data);
void __cdecl ReleaseCollisionResource(void** p_data);
LegoU8 __fastcall ReadModelCollisionChunk(
	ModelResource* p_model,
	void* p_unusedEdx,
	WinFile* p_file,
	LegoU32 p_tag,
	LegoU32 p_bytes
);
void __fastcall ReadModelShadowChunk(
	ModelResource* p_model,
	void* p_unusedEdx,
	WinFile* p_file,
	LegoU32 p_tag,
	LegoU32 p_bytes
);
void __fastcall FinalizeModelDrawPlans(ModelResource* p_model, void* p_unusedEdx);

#endif
