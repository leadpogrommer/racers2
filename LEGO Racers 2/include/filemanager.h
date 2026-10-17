#ifndef LR2_FILEMANAGER_H
#define LR2_FILEMANAGER_H

#include "decomp.h"
#include "types.h"
#include "winfile.h"

LegoS32 GetFileStorageMode();

class FileManager {
public:
	FileManager();
	~FileManager() {}
	void Initialize(BOOL p_force);
	BOOL FindArchiveEntry(LPCSTR p_path, BOOL* p_found, WinFile* p_file);
	void TrackOpen(LPCSTR p_path, WinFile* p_file);
	void Read(WinFile* p_file, LPVOID p_buffer, DWORD p_bytes, DWORD p_flags);
	void Close(WinFile* p_file);
	DWORD GetSize(WinFile* p_file);
	BOOL Exists(LPCSTR p_path);
	DWORD SeekCurrent(WinFile* p_file, LONG p_offset);
	void SetFlags(LegoU32 p_mask, LegoBool32 p_enabled);

	LegoS32 m_mode;                    // 0x00
	const LegoChar* m_archivePath;     // 0x04
	const LegoChar* m_fileListPath;    // 0x08
	const LegoChar* m_compressionPath; // 0x0c
	LegoU32 m_unk0x10;                 // 0x10, not initialized by the constructor
	LegoU32 m_archiveSize;             // 0x14
	void* m_unk0x18;                   // 0x18
	LegoS32 m_unk0x1c;                 // 0x1c
	void* m_unk0x20;                   // 0x20
	LegoS32 m_unk0x24;                 // 0x24
	LegoU32 m_unk0x28;                 // 0x28
	WinFile m_archive;                 // 0x2c
	LegoS32 m_archivePosition;         // 0x44
	LegoU32 m_flags;                   // 0x48
};

DECOMP_SIZE_ASSERT(FileManager, 0x4c);

struct ArchiveFileEntry {
	LegoChar m_name[128];
	LegoU32 m_archiveOffset;
	LegoU32 m_size;
};

DECOMP_SIZE_ASSERT(ArchiveFileEntry, 0x88);

FileManager* GetFileManager();

#endif // LR2_FILEMANAGER_H
