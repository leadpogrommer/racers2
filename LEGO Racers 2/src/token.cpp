#include "token.h"

// FUNCTION: LEGORACERS2 0x0045dcb0
LegoS32 FUN_0045dcb0(LegoChar** p_cursor, LegoChar* p_end)
{
	LegoBool32 comment;
	LegoBool32 tokenCharacter;
	LegoBool32 horizontalSpace;
	LegoBool32 firstCharacter = TRUE;
	tokenCharacter = FALSE;
	horizontalSpace = FALSE;
	comment = FALSE;

	while (*p_cursor < p_end) {
		LegoChar character = **p_cursor;
		if (character == '\r') {
			if ((*p_cursor)[1] == '\n') {
				*p_cursor += 2;
			}
			else {
				++*p_cursor;
			}
			break;
		}
		if (character == '\n') {
			++*p_cursor;
			break;
		}

		if (firstCharacter) {
			if (character == '#') {
				comment = TRUE;
			}
			firstCharacter = FALSE;
		}

		if (IsTokenCharacter(character)) {
			tokenCharacter = TRUE;
		}
		else if (IsHorizontalSpace(character)) {
			horizontalSpace = TRUE;
		}
		++*p_cursor;
	}

	if (comment) {
		return 2;
	}
	if (tokenCharacter) {
		return 0;
	}
	if (horizontalSpace) {
		return 1;
	}
	return 3;
}

// FUNCTION: LEGORACERS2 0x0045dda0
LegoBool32 IsTokenCharacter(LegoChar p_character)
{
	if ((p_character >= 'A' && p_character <= 'Z') || (p_character >= 'a' && p_character <= 'z') ||
		(p_character >= '0' && p_character <= '9') || p_character == '.' || p_character == '\\' || p_character == '"' ||
		p_character == '/' || p_character == '-' || p_character == '_' || p_character == '$' || p_character == '?' ||
		p_character == ',' || p_character == '!' || p_character == '\'') {
		return TRUE;
	}

	return FALSE;
}

// FUNCTION: LEGORACERS2 0x0045de00
LegoBool32 IsHorizontalSpace(LegoChar p_character)
{
	if (p_character == ' ' || p_character == '\t') {
		return TRUE;
	}

	return FALSE;
}

// STUB: LEGORACERS2 0x0045de20
void FUN_0045de20(
	LegoChar** p_cursor,
	LegoChar* p_end,
	LegoChar* p_output,
	LegoBool32 p_includeSpace,
	LegoBool32 p_trimTrailingSpace,
	LegoBool32 p_skipComments
)
{
	LegoBool32 seenToken = FALSE;
	LegoS32 outputLength = 0;

	while (TRUE) {
		if (*p_cursor > p_end) {
			break;
		}
		LegoChar character = **p_cursor;
		if (character == '\r') {
			if ((*p_cursor)[1] == '\n') {
				*p_cursor += 2;
			}
			else {
				++*p_cursor;
			}
			if (outputLength != 0) {
				break;
			}
			continue;
		}
		if (character == '\n') {
			++*p_cursor;
			if (outputLength != 0) {
				break;
			}
			continue;
		}

		if (character != '#' || p_skipComments != TRUE) {
			if (IsHorizontalSpace(character)) {
				if (seenToken && !p_includeSpace) {
					break;
				}
				if (seenToken) {
					p_output[outputLength++] = character;
				}
				++*p_cursor;
				continue;
			}

			if (IsTokenCharacter(character)) {
				seenToken = TRUE;
				p_output[outputLength] = character;
				++outputLength;
			}
			++*p_cursor;
		}
		else {
			LegoS32 lineType = FUN_0045dcb0(p_cursor, p_end);
			if (lineType == 3) {
				p_output[0] = '\0';
				return;
			}
		}
	}

	p_output[outputLength] = '\0';
	if (p_trimTrailingSpace) {
		while (outputLength > 0 && IsHorizontalSpace(p_output[outputLength - 1])) {
			p_output[--outputLength] = '\0';
		}
	}
}
