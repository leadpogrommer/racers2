#include "filemanager.h"

#include "charbuffer128.h"
#include "gtccompression.h"
#include "memoryheap.h"
#include "winfileutil.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x005498ec
const LegoChar* g_compressionInfoPath = "Compress.Inf";

// GLOBAL: LEGORACERS2 0x00578b60
FileManager* g_fileManager = NULL;

// GLOBAL: LEGORACERS2 0x00578b64
static LegoU32 g_fileManagerInitialized = 0;

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

// STUB: LEGORACERS2 0x004a25c0
void FileManager::Initialize(BOOL p_force)
{
	if (!p_force) {
		if (g_fileManagerInitialized != 0) {
			return;
		}
		g_fileManagerInitialized = 1;
	}

	m_flags = 0;
	if (m_mode != 3) {
		return;
	}

	WinFile fileList(m_fileListPath, 0, NULL);
	DWORD fileListSize = fileList.GetManagedSize();
	m_unk0x20 = GetMemoryHeap()->Allocate(fileListSize, "cFileCat: File entry list");
	m_unk0x24 = fileListSize / sizeof(ArchiveFileEntry);
	fileList.Read(m_unk0x20, fileListSize, 1);

	ArchiveFileEntry* entries = (ArchiveFileEntry*) m_unk0x20;
	for (LegoS32 index = 0; index < m_unk0x24; ++index) {
		((CharBuffer128*) entries[index].m_name)->Uppercase();
	}

	m_archive.Open(m_archivePath, 0, NULL);
}

// STUB: LEGORACERS2 0x004a2810
void FileManager::TrackOpen(LPCSTR p_path, WinFile* p_file)
{
	Initialize(FALSE);
	if (m_mode != 1 || p_path == m_archivePath || p_path == m_fileListPath) {
		return;
	}
	// The unique-file transfer path is reached only in storage mode 1.
	(void) p_file;
}

// STUB: LEGORACERS2 0x004a2a30
BOOL FileManager::FindArchiveEntry(LPCSTR p_path, BOOL* p_found, WinFile* p_file)
{
	if (m_mode < 2 || p_path == m_archivePath || p_path == m_fileListPath || p_path == m_compressionPath ||
		(m_flags & 1) != 0) {
		return FALSE;
	}

	Initialize(FALSE);
	CharBuffer128 searchName(p_path);
	searchName.Uppercase();

	LegoU32 firstIndex = m_unk0x28;
	if (firstIndex != 0) {
		--firstIndex;
	}

	ArchiveFileEntry* entries = (ArchiveFileEntry*) m_unk0x20;
	for (LegoS32 offset = 0; offset < m_unk0x24; ++offset) {
		LegoS32 index = (offset + firstIndex) % m_unk0x24;
		LegoS32 charIndex;
		for (charIndex = 0; charIndex < 128; ++charIndex) {
			LegoU8 left = (LegoU8) searchName.m_data[charIndex];
			LegoU8 right = (LegoU8) entries[index].m_name[charIndex];
			if (left >= 'a' && left <= 'z') {
				left &= 0xdf;
			}
			if (right >= 'a' && right <= 'z') {
				right &= 0xdf;
			}
			if (left != right) {
				break;
			}
			if (left == 0) {
				break;
			}
		}

		if (charIndex == 128 || searchName.m_data[charIndex] == entries[index].m_name[charIndex]) {
			m_unk0x28 = index;
			p_file->m_cachedSize = entries[index].m_size;
			p_file->m_archiveOffset = entries[index].m_archiveOffset;
			p_file->m_position = 0;
			*p_found = TRUE;
			return TRUE;
		}
	}

	*p_found = FALSE;
	return TRUE;
}

// STUB: LEGORACERS2 0x004a2ba0
BOOL FileManager::Read(WinFile* p_file, LPVOID p_buffer, DWORD p_bytes, DWORD p_flags)
{
	DWORD sourceOffset = p_file->m_archiveOffset + p_file->m_position;
	if (p_file->m_cachedSize == (DWORD) -1) {
		return p_file->ReadAt(p_buffer, p_bytes, sourceOffset, p_flags);
	}

	if (sourceOffset != (DWORD) m_archivePosition && !m_archive.m_compressed) {
		m_archive.SeekBounded(sourceOffset);
	}
	BOOL result = m_archive.ReadAt(p_buffer, p_bytes, sourceOffset, p_flags);
	p_file->m_position += p_bytes;
	m_archivePosition = p_file->m_archiveOffset + p_file->m_position;
	return result;
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

// STUB: LEGORACERS2 0x004a2c60
BOOL FileManager::Exists(LPCSTR p_path)
{
	if (m_mode >= 2 && (m_flags & 1) == 0) {
		Initialize(FALSE);
		CharBuffer128 searchName(p_path);
		searchName.Uppercase();
		ArchiveFileEntry* entries = (ArchiveFileEntry*) m_unk0x20;
		for (LegoS32 index = 0; index < m_unk0x24; ++index) {
			if (memcmp(searchName.m_data, entries[index].m_name, sizeof(searchName.m_data)) == 0) {
				return TRUE;
			}
		}
		return FALSE;
	}
	return PathExists(p_path);
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

struct GtcChunkRecord {
	LegoU32 m_archiveOffset;
	LegoU32 m_sourceOffset;
	LegoU32 m_storedSize;
	LegoU32 m_unk0x0c;
	LegoU32 m_unk0x10;
};

DECOMP_SIZE_ASSERT(GtcChunkRecord, 0x14);

class GtcBlockCache {
public:
	GtcBlockCache();
	~GtcBlockCache();
	void LoadBlock(LegoS32 p_index);

	LegoS32 m_currentIndex;
	LegoS32 m_recordCount;
	LegoS32 m_archivePosition;
	GtcChunkRecord* m_records;
	LegoU8* m_buffer;
};

DECOMP_SIZE_ASSERT(GtcBlockCache, 0x14);

// GLOBAL: LEGORACERS2 0x00578ca8
GtcBlockCache g_gtcBlockCache;

// STUB: LEGORACERS2 0x004a4820
GtcBlockCache::GtcBlockCache()
	: m_currentIndex(-1), m_recordCount(-1), m_archivePosition(-1), m_records(NULL), m_buffer(NULL)
{
	if (GetFileStorageMode() >= 2) {
		WinFile compressionInfo(g_compressionInfoPath, 0, NULL);
		DWORD infoSize = compressionInfo.GetManagedSize();
		m_recordCount = infoSize / sizeof(GtcChunkRecord);
		m_records = (GtcChunkRecord*) GetMemoryHeap()->Allocate(infoSize, "sCompressedChunkInfo array");
		compressionInfo.Read(m_records, infoSize, 1);
		m_buffer = (LegoU8*) GetMemoryHeap()->Allocate(0x20080, "Compressed data buffer");
	}
}

// STUB: LEGORACERS2 0x004a48e0
GtcBlockCache::~GtcBlockCache()
{
	if (m_buffer != NULL) {
		GetMemoryHeap()->Free(m_buffer);
		m_buffer = NULL;
	}
	if (m_records != NULL) {
		GetMemoryHeap()->Free(m_records);
		m_records = NULL;
	}
}

// STUB: LEGORACERS2 0x004a4930
void GtcBlockCache::LoadBlock(LegoS32 p_index)
{
	if (m_currentIndex == p_index) {
		return;
	}
	m_currentIndex = p_index;
	GtcChunkRecord* record = &m_records[p_index];
	FileManager* manager = GetFileManager();
	if (m_archivePosition != (LegoS32) record->m_archiveOffset) {
		manager->m_archive.SeekBounded(record->m_archiveOffset);
	}

	if ((LegoS32) record->m_storedSize < 0x20000) {
		LegoU8* storedData = m_buffer + 0x20040 - record->m_storedSize;
		manager->m_archive.ReadExact(storedData, record->m_storedSize, record->m_archiveOffset, 1);
		DecompressGtcBlock(m_buffer + 0x20040 - record->m_sourceOffset, m_buffer, FALSE);
	}
	else {
		manager->m_archive.ReadExact(m_buffer, 0x20000, record->m_archiveOffset, 1);
	}
	m_archivePosition = record->m_archiveOffset + record->m_storedSize;
}

// STUB: LEGORACERS2 0x004a49f0
BOOL __stdcall ReadCompressedData(LPVOID p_buffer, DWORD p_bytes, DWORD p_offset, DWORD p_flags)
{
	LegoS32 blockIndex = (LegoS32) p_offset >> 17;
	DWORD offsetInBlock = p_offset & 0x1ffff;
	DWORD bytesUntilBlockEnd = 0x20000 - offsetInBlock;
	LegoU8* output = (LegoU8*) p_buffer;
	LegoS32 bytesRemaining = p_bytes;

	(void) p_flags;
	if (bytesRemaining > 0) {
		do {
			g_gtcBlockCache.LoadBlock(blockIndex);
			DWORD bytesToCopy = bytesRemaining;
			if (bytesToCopy > bytesUntilBlockEnd) {
				bytesToCopy = bytesUntilBlockEnd;
			}
			memcpy(output, g_gtcBlockCache.m_buffer + offsetInBlock, bytesToCopy);
			bytesRemaining -= bytesToCopy;
			output += bytesToCopy;
			if (bytesRemaining < 1) {
				break;
			}
			bytesUntilBlockEnd = 0x20000;
			++blockIndex;
			offsetInBlock = 0;
		} while (TRUE);
	}
	return TRUE;
}
