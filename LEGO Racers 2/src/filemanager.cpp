#include "filemanager.h"

#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x005498ec
const LegoChar* g_compressionInfoPath = "Compress.Inf";

// GLOBAL: LEGORACERS2 0x00578b60
FileManager* g_fileManager = NULL;

// FUNCTION: LEGORACERS2 0x0049ada0
LegoS32 GetFileStorageMode()
{
	return 3;
}

// STUB: LEGORACERS2 0x004a2500
FileManager* GetFileManager()
{
	if (g_fileManager == NULL) {
		// Retail storage: 0x00578b10; VC6 omits this object's PDB data symbol.
		static FileManager manager;
		g_fileManager = &manager;
	}
	return g_fileManager;
}

// FUNCTION: LEGORACERS2 0x004a2560
FileManager::FileManager()
	: m_archiveSize(0), m_unk0x18(NULL), m_unk0x1c(0), m_unk0x20(NULL), m_unk0x24(0), m_unk0x28(0),
	  m_archivePosition(-1), m_flags(0)
{
	m_mode = GetFileStorageMode();
	m_archivePath = "GameData.Dat";
	m_fileListPath = "FileList.Inf";
	m_compressionPath = g_compressionInfoPath;
}

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

// STUB: LEGORACERS2 0x004a2d20
DWORD FileManager::SeekCurrent(WinFile* p_file, LONG p_offset)
{
	if (p_file->m_cachedSize == (DWORD) -1) {
		return p_file->SeekCurrent(p_offset);
	}
	{
		DWORD archivePosition = p_file->m_position + p_offset + p_file->m_archiveOffset;
		if (archivePosition != (DWORD) m_archivePosition && !m_archive.m_compressed) {
			m_archive.SeekBounded(archivePosition);
		}
	}
	p_file->m_position += p_offset;
	m_archivePosition = p_file->m_position + p_file->m_archiveOffset;
	return p_file->m_position;
}

// FUNCTION: LEGORACERS2 0x004a2d80
void FileManager::SetFlags(LegoU32 p_mask, LegoBool32 p_enabled)
{
	if (p_enabled) {
		m_flags |= p_mask;
	}
	else {
		m_flags &= ~p_mask;
	}
}
