#include "bodypartmanager.h"

#include "gameobjectdispatch.h"
#include "memoryheap.h"
#include "token.h"
#include "tokenstrings.h"
#include "winfile.h"

#include <new.h>
#include <string.h>

namespace
{
struct BodypartCategory {
	const LegoChar* m_prefix;
	const LegoChar* m_namesTag;
	const LegoChar* m_suffix;
	LegoU32 m_recordStride;
	LegoU32 m_countOffset;
	LegoU32 m_recordsOffset;
	LegoBool32 m_hasStoreFlag;
};

const BodypartCategory g_bodypartCategories[8] = {
	{"game data\\characters\\heads\\models\\", "head model names", ".MD2", 0x88, 0x458, 0x45c, TRUE},
	{"game data\\characters\\bodies\\textures\\", "torso texture names", " torso.TGA", 0x84, 0x460, 0x464, FALSE},
	{"game data\\characters\\bodies\\textures\\", "leg texture names", " legs.TGA", 0x84, 0x468, 0x46c, FALSE},
	{"game data\\characters\\heads\\textures\\", "face texture names", " face.TGA", 0x84, 0x470, 0x478, FALSE},
	{"game data\\Aliens\\alien heads\\models\\", "alienhead names", ".MD2", 0x88, 0x47c, 0x480, FALSE},
	{"game data\\Aliens\\Alien Bodies\\textures\\", "alientorso names", " torso.TGA", 0x84, 0x484, 0x488, FALSE},
	{"game data\\Ramas\\Rama heads\\models\\", "Ramahead names", ".MD2", 0x88, 0x48c, 0x490, FALSE},
	{"game data\\Ramas\\Rama Bodies\\textures\\", "Ramatorso names", " torso.TGA", 0x84, 0x494, 0x498, FALSE},
};

LegoU32 GetBodypartRecordCount(const LegoChar* p_fileData, LegoU32 p_size)
{
	LegoChar* cursor = (LegoChar*) p_fileData;
	LegoChar* end = cursor + p_size - 1;
	LegoU32 count = 0;

	while (cursor < end) {
		LegoS32 lineType = FUN_0045dcb0(&cursor, end);
		if (lineType == 3) {
			break;
		}
		if (lineType == 0) {
			++count;
		}
	}
	return count;
}

LegoBool32 IsStoreRecord(LegoChar* p_text)
{
	if (strcmp(p_text, "store") != 0) {
		return FALSE;
	}

	LegoChar* cursor = p_text + strlen(p_text) - 1;
	while (cursor != p_text) {
		if (*cursor > '/' && *cursor < '{') {
			cursor[1] = '\0';
			break;
		}
		--cursor;
	}
	return TRUE;
}

} // namespace

extern const BodypartManagerDispatch g_bodypartManagerDispatch = {
	&GameObjectBaseDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&BodypartManagerInitialize,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&GameObjectBaseRemove,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument,
	&BodypartManagerGetObjectSize,
};

BodypartManager::BodypartManager(BodypartManagerTemplate* p_template) : GameObject(0x900, &p_template->m_base, NULL)
{
	LegoS32 index;
	for (index = 0; index < 8; ++index) {
		m_paths[index] = p_template->m_paths[index];
	}
}

GameObject* __fastcall ConstructBodypartManager(void* p_storage, void*, BodypartManagerTemplate* p_template)
{
	BodypartManager* manager = new (p_storage) BodypartManager(p_template);
	manager->m_vtable = (const GameObjectDispatchView*) &g_bodypartManagerDispatch;
	return manager;
}

// FUNCTION: LEGORACERS2 0x00407f30
LegoU32 __fastcall BodypartManagerGetObjectSize(GameObject*, void*)
{
	return 0x49c;
}

// STUB: LEGORACERS2 0x0044cd00
void __fastcall BodypartManagerInitialize(GameObject* p_object, void*)
{
	BodypartManager* p_manager = (BodypartManager*) p_object;
	LegoS32 categoryIndex;
	for (categoryIndex = 0; categoryIndex < 8; ++categoryIndex) {
		const BodypartCategory& category = g_bodypartCategories[categoryIndex];
		LegoU32 sourceStorage[(sizeof(WinFile) + sizeof(LegoU32) - 1) / sizeof(LegoU32)];
		WinFile* source = new (sourceStorage) WinFile(p_manager->m_paths[categoryIndex].m_data, 0, NULL);
		DWORD fileSize = source->GetManagedSize();
		LegoChar* fileData = (LegoChar*) GetMemoryHeap()->AllocateFromEnd(fileSize + 0x40, category.m_namesTag);
		source->Read(fileData, fileSize, 1);
		source->~WinFile();

		LegoU32 recordCount = GetBodypartRecordCount(fileData, fileSize);
		LegoU8* records = (LegoU8*) AllocateHeapMemory(recordCount * category.m_recordStride);
		*(LegoU8**) ((LegoU8*) p_manager + category.m_recordsOffset) = records;
		LegoU32 recordIndex;
		for (recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
			CharBuffer128* path = (CharBuffer128*) (records + recordIndex * category.m_recordStride);
			new (path) CharBuffer128;
		}

		LegoChar* cursor = fileData;
		LegoChar* end = fileData + fileSize - 1;
		LegoU32 outputIndex = 0;
		for (recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
			LegoChar token[256];
			LegoBool32 isStore = FALSE;
			(void) FUN_0045de20(&cursor, end, token, TRUE, TRUE, TRUE);
			if (category.m_hasStoreFlag != FALSE) {
				isStore = IsStoreRecord(token);
			}

			FUN_0045df40(token, (LegoChar*) category.m_prefix);
			FUN_0045dfd0(token, (LegoChar*) category.m_suffix);
			CharBuffer128* output = (CharBuffer128*) (records + outputIndex * category.m_recordStride);
			output->SetText(token);
			LegoU32 flag = (categoryIndex == 4 || categoryIndex == 6) ? 0 : (LegoU32) -1;
			if (category.m_hasStoreFlag != FALSE) {
				flag = isStore != FALSE;
			}
			*(LegoU32*) (records + outputIndex * category.m_recordStride + 0x80) = flag;
			++outputIndex;
		}

		GetMemoryHeap()->Free(fileData);
		if (categoryIndex == 3) {
			p_manager->m_faceTextureCount = (LegoS32) (recordCount >> 3);
			p_manager->m_faceTextureGroupSize = 8;
		}
		else {
			*(LegoS32*) ((LegoU8*) p_manager + category.m_countOffset) = (LegoS32) recordCount;
		}
	}

	p_manager->GameObject::Initialize();
}
