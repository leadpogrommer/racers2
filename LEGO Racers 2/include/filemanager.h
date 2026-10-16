#ifndef LR2_FILEMANAGER_H
#define LR2_FILEMANAGER_H

#include "winfile.h"

class FileManager {
public:
	void Close(WinFile* p_file);
	DWORD GetSize(WinFile* p_file);
};

#endif // LR2_FILEMANAGER_H
