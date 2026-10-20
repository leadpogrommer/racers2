#ifndef LR2_WINFILEUTIL_H
#define LR2_WINFILEUTIL_H

#include "types.h"

LegoBool32 PathExists(const LegoChar* p_path);
LegoS32 GetPathDriveKind(const LegoChar* p_path);

#endif // LR2_WINFILEUTIL_H
