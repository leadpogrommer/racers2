#include "commandline.h"

#include "startup.h"

#include <stdlib.h>
#include <string.h>

// FUNCTION: LEGORACERS2 0x004b49e0
LegoChar* FUN_004b49e0(LegoChar* p_text, LegoChar* p_search)
{
	LegoChar textCopy[256];
	LegoChar searchCopy[256];
	LegoChar* p_found;

	strncpy(textCopy, p_text, sizeof(textCopy));
	strncpy(searchCopy, p_search, sizeof(searchCopy));
	_strlwr(textCopy);
	_strlwr(searchCopy);
	p_found = strstr(textCopy, searchCopy);
	if (p_found == NULL) {
		return NULL;
	}

	return p_text + (p_found - textCopy);
}

// FUNCTION: LEGORACERS2 0x004b4a60
LegoBool32 FUN_004b4a60(LegoChar* p_search)
{
	return FUN_004b49e0(g_commandLine, p_search) != NULL;
}

// FUNCTION: LEGORACERS2 0x004b4a80
LegoBool32 FUN_004b4a80(LegoChar* p_search, LegoChar* p_output, LegoS32 p_outputLength)
{
	LegoChar* p_commandEnd = g_commandLine + strlen(g_commandLine);
	LegoChar* p_outputBuffer = p_output;
	LegoChar* p_value;
	LegoBool32 quoted;
	LegoS32 outputLength;
	LegoS32 outputLimit;

	outputLength = 0;
	quoted = FALSE;
	p_value = FUN_004b49e0(g_commandLine, p_search);
	if (p_value == NULL) {
		return FALSE;
	}

	p_value += strlen(p_search);
	if (p_value < p_commandEnd) {
		if (*p_value == ' ') {
			++p_value;
		}
		if (*p_value == '"') {
			++p_value;
			quoted = TRUE;
		}

		if (p_value < p_commandEnd) {
			outputLimit = p_outputLength - 1;
			while (TRUE) {
				if (outputLength >= outputLimit) {
					break;
				}

				LegoChar character = *p_value;
				if (character == '"') {
					quoted = !quoted;
				}
				if (!quoted && (character == ' ' || character == '"')) {
					break;
				}

				p_outputBuffer[outputLength++] = character;
				++p_value;
				if (p_value < p_commandEnd) {
					continue;
				}
				break;
			}
		}
	}

	p_outputBuffer[outputLength] = '\0';
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004b4b30
LegoS32 FUN_004b4b30(LegoChar* p_search)
{
	LegoU8 value[16];
	LegoU8* p_value;

	p_value = value;
	if (FUN_004b4a80(p_search, (LegoChar*) value, sizeof(value))) {
		do {
			if (*p_value >= '0' && *p_value <= '9') {
				break;
			}
			++p_value;
		} while (p_value < value + sizeof(value) - 1);

		return atol((LegoChar*) p_value);
	}
	return 0;
}
