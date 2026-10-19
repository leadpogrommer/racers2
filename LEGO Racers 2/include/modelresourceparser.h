#ifndef LR2_MODELRESOURCEPARSER_H
#define LR2_MODELRESOURCEPARSER_H

#include "types.h"

class WinFile;
struct ModelResource;

void __fastcall ReadModelResourceGeometry(
	ModelResource* p_model,
	void* p_unusedEdx,
	const LegoChar* p_path,
	WinFile* p_file,
	LegoU32 p_uncached,
	LegoU32 p_flags
);

#endif
