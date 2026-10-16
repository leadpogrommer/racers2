#ifndef LR2_WINFILE_H
#define LR2_WINFILE_H

#include <windows.h>

class WinFile {
public:
	BOOL ReadExact(LPVOID p_buffer, DWORD p_bytes, DWORD p_unk1, DWORD p_unk2);
	BOOL WriteExact(LPCVOID p_buffer, DWORD p_bytes);
	DWORD GetSize();
	void Close();
	BOOL CreateAlways(LPCSTR p_path);
	BOOL OpenOrCreate(LPCSTR p_path);
	BOOL OpenAppend(LPCSTR p_path);
	DWORD SeekEnd();
	DWORD SeekCurrent(LONG p_offset);

	HANDLE m_handle;    // 0x00
	DWORD m_cachedSize; // 0x04
};

#endif // LR2_WINFILE_H
