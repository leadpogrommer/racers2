#include "applicationpolicy.h"

LegoBool32 IsApplicationDevelopmentMode()
{
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x00403930
void WINAPI OnApplicationDiagnostic(const LegoChar* p_message)
{
}

// FUNCTION: LEGORACERS2 0x0049a7e0
LegoU32 GetApplicationHeapReserve()
{
	return 0x01900000;
}

// FUNCTION: LEGORACERS2 0x0049a7f0
LegoU32 GetApplicationIconResourceId()
{
	return 101;
}
