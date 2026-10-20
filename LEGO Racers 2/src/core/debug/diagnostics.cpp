#include "core/debug/diagnostics.h"

#include "core/application/application.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

// GLOBAL: LEGORACERS2 0x006843b0
LegoBool32 g_diagnosticDialogAccepted = FALSE;

static LegoBool32 ShowDiagnosticDialog(const LegoChar* p_message);

// STUB: LEGORACERS2 0x004ab2a0
void ReportFatalError(const LegoChar* p_format, ...)
{
	LegoChar message[256];
	va_list arguments;
	va_start(arguments, p_format);
	vsprintf(message, p_format, arguments);
	va_end(arguments);
	if (strlen(message) > sizeof(message)) {
		ReportFatalError("TempString too small in Fatal()");
	}
	// Retail's DBUG-file logging is disabled by the shipped storage mode (3).
	ShowDiagnosticDialog(message);
	DebugBreak();
}

// STUB: LEGORACERS2 0x004ab3b0
void PrintDiagnostic(const LegoChar* p_format, ...)
{
	LegoChar message[256];
	va_list arguments;
	va_start(arguments, p_format);
	vsprintf(message, p_format, arguments);
	va_end(arguments);
	if (strlen(message) > sizeof(message)) {
		ReportFatalError("TempString too small in MyPrintf()");
	}
	GetApplication()->NotifyDiagnostic(message);
}

// FUNCTION: LEGORACERS2 0x004e7b50
static LegoBool32 ShowDiagnosticDialog(const LegoChar* p_message)
{
	LegoBool32 accepted = MessageBox(NULL, p_message, "Error", 0x11041) == IDOK;
	g_diagnosticDialogAccepted = accepted;
	return accepted;
}
