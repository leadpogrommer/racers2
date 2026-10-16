#include "winfile.h"

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
