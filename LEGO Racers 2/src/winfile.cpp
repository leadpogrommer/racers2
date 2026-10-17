#include "winfile.h"

#include "application.h"
#include "filemanager.h"

#include <string.h>

BOOL __stdcall ReadCompressedData(LPVOID p_buffer, DWORD p_bytes, DWORD p_offset, DWORD p_flags);

// GLOBAL: LEGORACERS2 0x00549b58
LegoBool32 g_checkArchiveRedirect = TRUE;

// FUNCTION: LEGORACERS2 0x004a21b0
WinFile::WinFile(LPCSTR p_path, LegoS32 p_mode, BOOL* p_result)
{
	m_handle = NULL;
	m_cachedSize = (DWORD) -1;
	m_archiveOffset = 0;
	m_position = 0;
	m_unk0x10 = (DWORD) -1;
	m_compressed = FALSE;
	Open(p_path, p_mode, p_result);
}

// STUB: LEGORACERS2 0x004a21f0
void WinFile::Open(LPCSTR p_path, LegoS32 p_mode, BOOL* p_result)
{
	BOOL result = FALSE;
	FileManager* manager;
	BOOL found;

	switch (p_mode) {
	case 0:
		manager = GetFileManager();
		if (!manager->FindArchiveEntry(p_path, &found, this)) {
			result = OpenPath(p_path);
			if (result) {
				GetFileManager()->TrackOpen(p_path, this);
			}
		}
		else {
			result = found;
		}
		break;
	case 1:
		result = CreateAlways(p_path);
		break;
	case 2:
		result = OpenOrCreate(p_path);
		break;
	case 3:
		result = OpenAppend(p_path);
		break;
	}

	if (p_result != NULL) {
		*p_result = result;
	}
}

// FUNCTION: LEGORACERS2 0x004a2290
void WinFile::Read(LPVOID p_buffer, DWORD p_bytes, DWORD p_flags)
{
	GetFileManager()->Read(this, p_buffer, p_bytes, p_flags);
}

// FUNCTION: LEGORACERS2 0x004a22b0
void WinFile::CloseManaged()
{
	GetFileManager()->Close(this);
}

// FUNCTION: LEGORACERS2 0x004a22c0
DWORD WinFile::GetManagedSize()
{
	return GetFileManager()->GetSize(this);
}

// FUNCTION: LEGORACERS2 0x004a22d0
WinFile::~WinFile()
{
	CloseManaged();
}

// FUNCTION: LEGORACERS2 0x004a22e0
DWORD WinFile::SeekManagedCurrent(LONG p_offset)
{
	return GetFileManager()->SeekCurrent(this, p_offset);
}

// FUNCTION: LEGORACERS2 0x004a2300
BOOL ManagedPathExists(LPCSTR p_path)
{
	return GetFileManager()->Exists(p_path);
}

// STUB: LEGORACERS2 0x004a4570
BOOL WinFile::OpenPath(LPCSTR p_path)
{
	LPCSTR openPath = p_path;
	if (g_checkArchiveRedirect) {
		if (!GetApplication()->IsDevelopmentMode() && _stricmp(p_path, "GameData.dat") == 0) {
			m_compressed = TRUE;
			openPath = "GameData.gtc";
			g_checkArchiveRedirect = FALSE;
		}
	}

	m_handle = CreateFile(
		openPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);
	while (m_handle == INVALID_HANDLE_VALUE && GetLastError() == ERROR_SHARING_VIOLATION) {
		Sleep(500);
		// Retail retries the originally requested name, even after the GTC redirect.
		m_handle = CreateFile(
			p_path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
		);
	}
	return m_handle != INVALID_HANDLE_VALUE;
}

// FUNCTION: LEGORACERS2 0x004a4620
BOOL WinFile::CreateAlways(LPCSTR p_path)
{
	m_handle = CreateFile(p_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	while (m_handle == INVALID_HANDLE_VALUE && GetLastError() == ERROR_LOCK_VIOLATION) {
		Sleep(500);
		m_handle =
			CreateFile(p_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return m_handle != INVALID_HANDLE_VALUE;
}

// FUNCTION: LEGORACERS2 0x004a46a0
BOOL WinFile::OpenOrCreate(LPCSTR p_path)
{
	m_handle = CreateFile(p_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	while (m_handle == INVALID_HANDLE_VALUE && GetLastError() == ERROR_LOCK_VIOLATION) {
		Sleep(500);
		m_handle = CreateFile(p_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return m_handle != INVALID_HANDLE_VALUE;
}

// FUNCTION: LEGORACERS2 0x004a4720
DWORD WinFile::SeekEnd()
{
	DWORD position = SetFilePointer(m_handle, 0, NULL, FILE_END);
	if (position == (DWORD) -1) {
		return 0;
	}
	return position;
}

// FUNCTION: LEGORACERS2 0x004a4740
DWORD WinFile::SeekBounded(LONG p_offset)
{
	LONG size = GetManagedSize();
	if (p_offset < -size) {
		p_offset = -size;
	}
	if (p_offset > size) {
		p_offset = size;
	}
	DWORD position;
	if (p_offset >= 0) {
		position = SetFilePointer(m_handle, p_offset, NULL, FILE_BEGIN);
	}
	else {
		position = SetFilePointer(m_handle, p_offset, NULL, FILE_END);
	}
	if (position == (DWORD) -1) {
		return 0;
	}
	return position;
}

// FUNCTION: LEGORACERS2 0x004a4790
DWORD WinFile::SeekCurrent(LONG p_offset)
{
	DWORD position = SetFilePointer(m_handle, p_offset, NULL, FILE_CURRENT);
	if (position == (DWORD) -1) {
		return 0;
	}
	return position;
}

// FUNCTION: LEGORACERS2 0x004a47b0
BOOL WinFile::OpenAppend(LPCSTR p_path)
{
	if (!OpenOrCreate(p_path)) {
		return FALSE;
	}
	if (m_handle != NULL) {
		SeekEnd();
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004a4a90
BOOL WinFile::ReadAt(LPVOID p_buffer, DWORD p_bytes, DWORD p_offset, DWORD p_flags)
{
	if (m_compressed) {
		ReadCompressedData(p_buffer, p_bytes, p_offset, p_flags);
		return TRUE;
	}
	return ReadExact(p_buffer, p_bytes, p_offset, p_flags);
}

// FUNCTION: LEGORACERS2 0x004a4ac0
BOOL WinFile::ReadExact(LPVOID p_buffer, DWORD p_bytes, DWORD p_unk1, DWORD p_unk2)
{
	DWORD bytesRead;
	if (!ReadFile(m_handle, p_buffer, p_bytes, &bytesRead, NULL)) {
		return FALSE;
	}
	return bytesRead == p_bytes;
}

// FUNCTION: LEGORACERS2 0x004a4b00
BOOL WinFile::WriteExact(LPCVOID p_buffer, DWORD p_bytes)
{
	DWORD bytesWritten;
	if (!WriteFile(m_handle, p_buffer, p_bytes, &bytesWritten, NULL)) {
		return FALSE;
	}
	return bytesWritten == p_bytes;
}

// FUNCTION: LEGORACERS2 0x004a4b40
DWORD WinFile::GetSize()
{
	DWORD size = GetFileSize(m_handle, NULL);
	if (size == INVALID_FILE_SIZE) {
		size = 0;
	}
	return size;
}

// FUNCTION: LEGORACERS2 0x004a4b60
void WinFile::Close()
{
	if (m_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(m_handle);
	}
	m_handle = INVALID_HANDLE_VALUE;
}
