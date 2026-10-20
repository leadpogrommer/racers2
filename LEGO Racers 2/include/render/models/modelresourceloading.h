#ifndef LR2_MODELRESOURCELOADING_H
#define LR2_MODELRESOURCELOADING_H

#include "render/models/modelresource.h"

// Retail 4a7d40: cdecl, four stack arguments and caller cleanup.
ModelResource* __cdecl LoadModelResource(
	const LegoChar* p_path,
	LegoS32 p_loadCollision,
	LegoS32 p_uncached,
	LegoU32 p_flags
);

#endif
