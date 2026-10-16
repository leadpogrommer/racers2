#include "filemanager.h"

// FUNCTION: LEGORACERS2 0x004a2c20
void FileManager::Close(WinFile* p_file)
{
	if (p_file->m_cachedSize == (DWORD) -1) {
		p_file->Close();
	}
}

// FUNCTION: LEGORACERS2 0x004a2c40
DWORD FileManager::GetSize(WinFile* p_file)
{
	if (p_file->m_cachedSize != (DWORD) -1) {
		return p_file->m_cachedSize;
	}
	return p_file->GetSize();
}
