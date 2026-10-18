#include "audioprovider.h"
#include "diagnostics.h"
#include "stereoaudioprovider.h"
#include "threedimensionalaudioprovider.h"

#include <new.h>

struct DirectSoundOutputProviderPrefix;
void __fastcall ReleaseDirectSoundOutputInterfaces(DirectSoundOutputProviderPrefix* p_provider, void* p_edx);

// STUB: LEGORACERS2 0x004ef930
ThreeDimensionalDirectSoundProvider::ThreeDimensionalDirectSoundProvider(HWND p_window)
{
	m_vtable = &g_threeDimensionalDirectSoundProviderDispatch;
	m_device = NULL;
	m_primaryBuffer = NULL;
	m_initializationStatus = 0;
	m_unk0x10 = 0;
	m_listener = NULL;
	m_unk0x18 = NULL;
	m_unk0x1c = NULL;
	m_unk0x20 = NULL;
	Initialize(p_window);
}

// STUB: LEGORACERS2 0x004ef9d0
void ThreeDimensionalDirectSoundProvider::Initialize(HWND p_window)
{
	HRESULT result = DirectSoundCreate8(NULL, &m_device, NULL);
	if (result < 0) {
		return;
	}

	result = m_device->SetCooperativeLevel(p_window, DSSCL_PRIORITY);
	if (result != DS_OK) {
		m_device->Release();
		return;
	}

	DSBUFFERDESC description;
	ZeroMemory(&description, sizeof(description));
	description.dwSize = 0x24;
	description.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
	result = m_device->CreateSoundBuffer(&description, &m_primaryBuffer, NULL);
	if (result != DS_OK) {
		m_device->Release();
		return;
	}

	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener, (void**) &m_listener);
	if (result != DS_OK) {
		ReportFatalError("DirectSound3D listener creation failed: 0x%08lx", result);
		m_device->Release();
		return;
	}

	m_listener->SetDistanceFactor(0.75f, 1);
	m_initializationStatus = TRUE;
}

// STUB: LEGORACERS2 0x004efab0
void ThreeDimensionalDirectSoundProvider::ReleaseDeviceInterfaces()
{
	if (m_primaryBuffer != NULL) {
		if (m_unk0x18 != NULL) {
			m_unk0x18->Release();
		}
		if (m_listener != NULL) {
			m_listener->Release();
		}
		if (m_unk0x20 != NULL) {
			m_unk0x20->Release();
		}
		if (m_unk0x1c != NULL) {
			m_unk0x1c->Release();
		}
		m_primaryBuffer->Release();
		m_primaryBuffer = NULL;
	}

	if (m_device != NULL) {
		m_device->Release();
		m_device = NULL;
	}
}

// STUB: LEGORACERS2 0x004f04e0
StereoDirectSoundProvider::StereoDirectSoundProvider(HWND p_window)
{
	m_vtable = &g_stereoDirectSoundProviderDispatch;
	m_device = NULL;
	m_primaryBuffer = NULL;
	m_initializationStatus = 0;
	Initialize(p_window);
}

// STUB: LEGORACERS2 0x004f0550
void StereoDirectSoundProvider::Initialize(HWND p_window)
{
	InitializeDirectSoundOutput(&m_device, &m_primaryBuffer, &m_initializationStatus, p_window, FALSE);
}

void StereoDirectSoundProvider::ReleaseDeviceInterfaces()
{
	ReleaseDirectSoundOutputInterfaces((DirectSoundOutputProviderPrefix*) this, NULL);
}

// Source-level placement adapters used by AudioService::Initialize.
void* ConstructStereoAudioProvider(void* p_storage, HWND p_window)
{
	return new (p_storage) StereoDirectSoundProvider(p_window);
}

void* ConstructThreeDimensionalAudioProvider(void* p_storage, HWND p_window)
{
	return new (p_storage) ThreeDimensionalDirectSoundProvider(p_window);
}
