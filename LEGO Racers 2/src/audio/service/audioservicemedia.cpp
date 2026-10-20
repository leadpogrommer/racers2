#include "audio/service/audioservice.h"
#include "core/debug/diagnostics.h"

#include <string.h>

// STUB: LEGORACERS2 0x004ef2a0
void AudioService::SetCdPlayerMode(LegoU32 p_mode)
{
	if (m_initializationFailed != 0 || m_shutdown != 0) {
		return;
	}
	if (m_usesDigitalAudio != 0) {
		return;
	}

	m_mciOpenParameters.lpstrDeviceType = (LPCSTR) 0x204;
	if (mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR) &m_mciOpenParameters) != 0) {
		MessageBoxA(NULL, "Couldn't open CD player", "Error", MB_ICONERROR);
		return;
	}

	mciSendCommandA(m_mciOpenParameters.wDeviceID, p_mode == 0 ? 0x855 : 0x809, 0, 0);
	mciSendCommandA(m_mciOpenParameters.wDeviceID, MCI_CLOSE, 0, 0);
}

// FUNCTION: LEGORACERS2 0x004ef360
void AudioService::SetDigitalAudioPath(LPCSTR p_path, LegoU32)
{
	if (m_initializationFailed != 0 || m_shutdown != 0) {
		return;
	}

	m_usesDigitalAudio = TRUE;
	strcpy((LegoChar*) m_unk0x24874, p_path);
}
