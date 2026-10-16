#include "tokenstrings.h"

#include <string.h>

// FUNCTION: LEGORACERS2 0x0045df40
void FUN_0045df40(LegoChar* p_destination, LegoChar* p_source)
{
	LegoChar buffer[256];
	strcpy(buffer, p_source);
	strcpy(buffer + strlen(p_source), p_destination);
	strcpy(p_destination, buffer);
}

// FUNCTION: LEGORACERS2 0x0045dfd0
void FUN_0045dfd0(LegoChar* p_destination, LegoChar* p_source)
{
	strcpy(p_destination + strlen(p_destination), p_source);
}
