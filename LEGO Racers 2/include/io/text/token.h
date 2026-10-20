#ifndef LR2_TOKEN_H
#define LR2_TOKEN_H

#include "types.h"

LegoBool32 IsTokenCharacter(LegoChar p_character);
LegoBool32 IsHorizontalSpace(LegoChar p_character);

LegoS32 FUN_0045dcb0(LegoChar** p_cursor, LegoChar* p_end);
void FUN_0045de20(
	LegoChar** p_cursor,
	LegoChar* p_end,
	LegoChar* p_output,
	LegoBool32 p_includeSpace,
	LegoBool32 p_trimTrailingSpace,
	LegoBool32 p_skipComments
);

#endif // LR2_TOKEN_H
