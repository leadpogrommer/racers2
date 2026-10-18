#include "audiocommentary.h"
#include "diagnostics.h"

// STUB: LEGORACERS2 0x004ef630
LegoBool32 __fastcall CheckAudioMediaResult(AudioService*, void*, HRESULT p_result)
{
	if (p_result == 0) {
		return FALSE;
	}
	const LegoChar* message;
	switch ((DWORD) p_result) {
	case 0x88780046:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x80040218:
		message = "Unable to find combination of Filter to suit file - DSHOW";
		break;
	case 0x80004005:
		message = "Failed: Unknown error - DSHOW";
		break;
	case 0x80004002:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x80004003:
		message = "Pointer Null - DSHOW";
		break;
	case 0x80004004:
		message = "Connect Pins Aborted - DSHOW";
		break;
	case 0x80040154:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x80040216:
		message = "Object has no name - DSHOW";
		break;
	case 0x80040217:
		message = "Cannot connect pins - DSHOW";
		break;
	case 0x8004025f:
		message = "NIG - DSHOW";
		break;
	case 0x8004022f:
		message = "Invalid file format -DSHOW";
		break;
	case 0x80040240:
		message = "Unknown file type - DSHOW";
		break;
	case 0x80040241:
		message = "Cannot load source filter - DSHOW";
		break;
	case 0x80040265:
		message = "Unsupported stream - DSHOW";
		break;
	case 0x8007000e:
		message = "Out of Memory";
		break;
	case 0x80070057:
		PrintDiagnostic("Invalid Argument - DSHOW");
		return TRUE;
	case 0x00040242:
		message = "Unable to find suitable renderer - DSHOW";
		break;
	case 0x88781226:
		message = "Performance cannot support audiopaths";
		break;
	case 0x0004022d:
		message = "Duplicate Name - DSHOW";
		break;
	case 0x00040257:
		message = "Unable to find suitable video renderer - DSHOW";
		break;
	case 0x00040258:
		message = "Unable to find suitable audio renderer - DSHOW";
		break;
	case 0x88781184:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x88781162:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x88780096:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x887800aa:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x88781151:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x88781180:
		message = "A required object is not initialized or failed to initialize";
		break;
	case 0x88781182:
		message = "Unable to open file";
		break;
	case 0x88781183:
		message = "A required object is not initialized or failed to initialize";
		break;
	default:
		message = "Uncaught error - DSHOW";
		break;
	}
	ReportFatalError(message);
	return TRUE;
}
