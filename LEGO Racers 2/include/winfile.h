#ifndef LR2_WINFILE_H
#define LR2_WINFILE_H

#include "types.h"

#include <windows.h>

class WinFile {
public:
	WinFile(LPCSTR p_path, LegoS32 p_mode, BOOL* p_result);
	WinFile()
		: m_handle(NULL), m_cachedSize((DWORD) -1), m_archiveOffset(0), m_position(0), m_unk0x10((DWORD) -1),
		  m_compressed(FALSE)
	{
	}
	~WinFile();
	void CloseManaged();
	DWORD GetManagedSize();
	BOOL ReadExact(LPVOID p_buffer, DWORD p_bytes, DWORD p_unk1, DWORD p_unk2);
	BOOL ReadAt(LPVOID p_buffer, DWORD p_bytes, DWORD p_offset, DWORD p_flags);
	BOOL Read(LPVOID p_buffer, DWORD p_bytes, DWORD p_flags);
	BOOL WriteExact(LPCVOID p_buffer, DWORD p_bytes);
	DWORD GetSize();
	void Close();
	BOOL CreateAlways(LPCSTR p_path);
	BOOL OpenOrCreate(LPCSTR p_path);
	BOOL OpenAppend(LPCSTR p_path);
	void Open(LPCSTR p_path, LegoS32 p_mode, BOOL* p_result);
	BOOL OpenPath(LPCSTR p_path);
	DWORD SeekEnd();
	DWORD SeekBounded(LONG p_offset);
	DWORD SeekCurrent(LONG p_offset);
	DWORD SeekManagedCurrent(LONG p_offset);

	HANDLE m_handle;       // 0x00
	DWORD m_cachedSize;    // 0x04
	DWORD m_archiveOffset; // 0x08
	DWORD m_position;      // 0x0c
	DWORD m_unk0x10;       // 0x10
	BOOL m_compressed;     // 0x14
};

BOOL ManagedPathExists(LPCSTR p_path);

#endif // LR2_WINFILE_H
