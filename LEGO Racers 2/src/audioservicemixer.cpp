#include "audiochannelreload.h"
#include "audioservice.h"
#include "diagnostics.h"

// STUB: LEGORACERS2 0x004eedf0
void AudioService::SetMixerVolume(LegoFloat p_volume, LegoU32 p_unused)
{
	if (m_initializationFailed != 0 || m_shutdown != 0) {
		return;
	}
	if (1.0f < p_volume || p_volume < 0.0f) {
		ReportFatalError("FPU precision prob %f", (double) p_volume);
	}
	if (m_usesDigitalAudio != 0) {
		SetDigitalMusicGain(this, NULL, p_volume, p_unused);
		return;
	}
	HMIXER mixer = NULL;
	UINT devices = mixerGetNumDevs();
	if (devices == 0 || devices >= 2) {
		return;
	}
	MIXERCAPSA capabilities;
	for (UINT i = 0; i < devices; ++i) {
		if (mixerGetDevCapsA(i, &capabilities, sizeof(capabilities)) != MMSYSERR_NOERROR) {
			ReportFatalError("Unable to open mixer device (CAPS)");
			return;
		}
	}
	if (mixerOpen(&mixer, 0, 0, 0, 0x10000) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to open mixer device (OPEN)");
	}
	MIXERLINEA line;
	line.cbStruct = sizeof(line);
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
	if (mixerGetLineInfoA((HMIXEROBJ) mixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to get line info - CDSETVOL");
	}
	MIXERCONTROLA control;
	MIXERLINECONTROLSA lineControls;
	lineControls.cbStruct = sizeof(lineControls);
	lineControls.dwLineID = line.dwLineID;
	lineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	lineControls.cbmxctrl = sizeof(control);
	lineControls.pamxctrl = &control;
	if (mixerGetLineControlsA((HMIXEROBJ) mixer, &lineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to open mixer device (OPEN)");
	}
	DWORD volume;
	MIXERCONTROLDETAILS details;
	details.cbStruct = sizeof(details);
	details.dwControlID = control.dwControlID;
	details.cChannels = 1;
	details.cMultipleItems = 0;
	details.cbDetails = sizeof(volume);
	details.paDetails = &volume;
	if (mixerGetControlDetailsA((HMIXEROBJ) mixer, &details, MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to get volume from mixer device");
	}
	volume = (DWORD) (p_volume * 65535.0f);
	if (volume > 0xffff) {
		ReportFatalError("Range check failed %u", volume);
	}
	if (mixerSetControlDetails((HMIXEROBJ) mixer, &details, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to set volume from mixer device");
	}
	if (mixerClose(mixer) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to close mixer device");
	}
}

// STUB: LEGORACERS2 0x004ef060
void AudioService::GetMixerVolume(LegoFloat* p_volumeA, LegoFloat* p_volumeB)
{
	if (m_initializationFailed != 0 || m_shutdown != 0) {
		return;
	}
	if (m_usesDigitalAudio != 0) {
		GetDigitalMusicGain(this, NULL, p_volumeA, p_volumeB);
		return;
	}
	HMIXER mixer = NULL;
	UINT devices = mixerGetNumDevs();
	if (devices == 0 || devices >= 2) {
		return;
	}
	MIXERCAPSA capabilities;
	for (UINT i = 0; i < devices; ++i) {
		if (mixerGetDevCapsA(i, &capabilities, sizeof(capabilities)) != MMSYSERR_NOERROR) {
			ReportFatalError("Unable to open mixer device (CAPS)");
			return;
		}
	}
	if (mixerOpen(&mixer, 0, 0, 0, 0x10000) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to open mixer device (OPEN)");
	}
	MIXERLINEA line;
	line.cbStruct = sizeof(line);
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
	if (mixerGetLineInfoA((HMIXEROBJ) mixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to get line info - CDSETVOL");
	}
	MIXERCONTROLA control;
	MIXERLINECONTROLSA lineControls;
	lineControls.cbStruct = sizeof(lineControls);
	lineControls.dwLineID = line.dwLineID;
	lineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	lineControls.cbmxctrl = sizeof(control);
	lineControls.pamxctrl = &control;
	if (mixerGetLineControlsA((HMIXEROBJ) mixer, &lineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to open mixer device (OPEN)");
	}
	DWORD channelValues[3];
	MIXERCONTROLDETAILS details;
	details.cbStruct = sizeof(details);
	details.dwControlID = control.dwControlID;
	details.cChannels = line.cChannels;
	details.cMultipleItems = control.cMultipleItems;
	details.cbDetails = sizeof(channelValues[0]);
	details.paDetails = channelValues;
	if (mixerGetControlDetailsA((HMIXEROBJ) mixer, &details, MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to get volume from mixer device");
	}
	LegoFloat value = (LegoFloat) channelValues[0] * (1.0f / 65535.0f);
	*p_volumeA = value;
	*p_volumeB = value;
	if (1.0f < *p_volumeA || *p_volumeA < 0.0f) {
		ReportFatalError("FPU precision prob %f", (double) *p_volumeA);
	}
	if (mixerClose(mixer) != MMSYSERR_NOERROR) {
		ReportFatalError("Unable to close mixer device");
	}
}
